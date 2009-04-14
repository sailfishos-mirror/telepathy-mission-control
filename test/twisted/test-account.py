import dbus
import dbus.service

from servicetest import EventPattern, tp_name_prefix, tp_path_prefix, \
        call_async
from mctest import exec_test, create_fakecm_account
import constants as cs

def test(q, bus, mc):
    # Get the AccountManager interface
    account_manager = bus.get_object(cs.AM, cs.AM_PATH)
    account_manager_iface = dbus.Interface(account_manager, cs.AM)

    # Introspect AccountManager for debugging purpose
    account_manager_introspected = account_manager.Introspect(
            dbus_interface=cs.INTROSPECTABLE_IFACE)
    #print account_manager_introspected

    # Check AccountManager has D-Bus property interface
    properties = account_manager.GetAll(cs.AM,
            dbus_interface=cs.PROPERTIES_IFACE)
    assert properties is not None
    assert properties.get('ValidAccounts') == [], \
        properties.get('ValidAccounts')
    assert properties.get('InvalidAccounts') == [], \
        properties.get('InvalidAccounts')
    interfaces = properties.get('Interfaces')

    # assert that current functionality exists
    assert cs.AM_IFACE_CREATION_DRAFT in interfaces, interfaces
    assert cs.AM_IFACE_NOKIA_QUERY in interfaces, interfaces

    params = dbus.Dictionary({"account": "someguy@example.com",
        "password": "secrecy"}, signature='sv')
    (cm_name_ref, account) = create_fakecm_account(q, bus, mc, params)

    account_path = account.__dbus_object_path__

    # Check the account is correctly created
    properties = account_manager.GetAll(cs.AM,
            dbus_interface=cs.PROPERTIES_IFACE)
    assert properties is not None
    assert properties.get('ValidAccounts') == [account_path], properties
    account_path = properties['ValidAccounts'][0]
    assert isinstance(account_path, dbus.ObjectPath), repr(account_path)
    assert properties.get('InvalidAccounts') == [], properties

    account_iface = dbus.Interface(account, cs.ACCOUNT)
    account_props = dbus.Interface(account, cs.PROPERTIES_IFACE)
    # Introspect Account for debugging purpose
    account_introspected = account.Introspect(
            dbus_interface=cs.INTROSPECTABLE_IFACE)
    #print account_introspected

    # Check Account has D-Bus property interface
    properties = account_props.GetAll(cs.ACCOUNT)
    assert properties is not None

    assert properties.get('DisplayName') == 'fakeaccount', \
        properties.get('DisplayName')
    assert properties.get('Icon') == '', properties.get('Icon')
    assert properties.get('Valid') == True, properties.get('Valid')
    assert properties.get('Enabled') == False, properties.get('Enabled')
    #assert properties.get('Nickname') == 'fakenick', properties.get('Nickname')
    assert properties.get('Parameters') == params, properties.get('Parameters')
    assert properties.get('Connection') == '/', properties.get('Connection')
    assert properties.get('NormalizedName') == '', \
        properties.get('NormalizedName')

    interfaces = properties.get('Interfaces')
    assert cs.ACCOUNT_IFACE_AVATAR in interfaces, interfaces
    assert cs.ACCOUNT_IFACE_NOKIA_COMPAT in interfaces, interfaces
    assert cs.ACCOUNT_IFACE_NOKIA_CONDITIONS in interfaces, interfaces

    # sanity check
    for k in properties:
        assert account_props.Get(cs.ACCOUNT, k) == properties[k], k

    # Alter some miscellaneous r/w properties

    call_async(q, account_props, 'Set', cs.ACCOUNT, 'DisplayName',
            'Work account')
    q.expect_many(
        EventPattern('dbus-signal',
            path=account_path,
            signal='AccountPropertyChanged',
            interface=cs.ACCOUNT,
            args=[{'DisplayName': 'Work account'}]),
        EventPattern('dbus-return', method='Set'),
        )
    assert account_props.Get(cs.ACCOUNT, 'DisplayName') == 'Work account'

    call_async(q, account_props, 'Set', cs.ACCOUNT, 'Icon', 'im-jabber')
    q.expect_many(
        EventPattern('dbus-signal',
            path=account_path,
            signal='AccountPropertyChanged',
            interface=cs.ACCOUNT,
            args=[{'Icon': 'im-jabber'}]),
        EventPattern('dbus-return', method='Set'),
        )
    assert account_props.Get(cs.ACCOUNT, 'Icon') == 'im-jabber'

    assert account_props.Get(cs.ACCOUNT, 'HasBeenOnline') == False
    call_async(q, account_props, 'Set', cs.ACCOUNT, 'Nickname', 'Joe Bloggs')
    q.expect_many(
        EventPattern('dbus-signal',
            path=account_path,
            signal='AccountPropertyChanged',
            interface=cs.ACCOUNT,
            args=[{'Nickname': 'Joe Bloggs'}]),
        EventPattern('dbus-return', method='Set'),
        )
    assert account_props.Get(cs.ACCOUNT, 'Nickname') == 'Joe Bloggs'

    call_async(q, dbus.Interface(account, cs.ACCOUNT_IFACE_NOKIA_COMPAT),
            'SetHasBeenOnline')
    q.expect_many(
        EventPattern('dbus-signal',
            path=account_path,
            signal='AccountPropertyChanged',
            interface=cs.ACCOUNT,
            args=[{'HasBeenOnline': True}]),
        EventPattern('dbus-return', method='SetHasBeenOnline'),
        )
    assert account_props.Get(cs.ACCOUNT, 'HasBeenOnline') == True

    # Delete the account
    assert account_iface.Remove() is None
    account_event, account_manager_event = q.expect_many(
        EventPattern('dbus-signal',
            path=account_path,
            signal='Removed',
            interface=cs.ACCOUNT,
            args=[]
            ),
        EventPattern('dbus-signal',
            path=cs.AM_PATH,
            signal='AccountRemoved',
            interface=cs.AM,
            args=[account_path]
            ),
        )

    # Check the account is correctly deleted
    properties = account_manager.GetAll(cs.AM,
            dbus_interface=cs.PROPERTIES_IFACE)
    assert properties is not None
    assert properties.get('ValidAccounts') == [], properties
    assert properties.get('InvalidAccounts') == [], properties


if __name__ == '__main__':
    exec_test(test, {})
