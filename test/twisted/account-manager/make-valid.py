"""Feature test for accounts becoming valid.
"""

import os

import dbus
import dbus.service

from servicetest import EventPattern, tp_name_prefix, tp_path_prefix, \
        call_async, sync_dbus
from mctest import exec_test, SimulatedConnection, create_fakecm_account, \
        make_mc
import constants as cs

cm_name_ref = dbus.service.BusName(
        cs.tp_name_prefix + '.ConnectionManager.fakecm', bus=dbus.SessionBus())

account1_id = 'fakecm/fakeprotocol/jc_2edenton_40unatco_2eint'
account2_id = 'fakecm/fakeprotocol/jc_2edenton_40example_2ecom'

def preseed():

    accounts_dir = os.environ['MC_ACCOUNT_DIR']

    # The passwords are missing, so the accounts can't connect yet.
    accounts_cfg = open(accounts_dir + '/accounts.cfg', 'w')
    accounts_cfg.write("""# Telepathy accounts
[%s]
manager=fakecm
protocol=fakeprotocol
DisplayName=Work account
NormalizedName=jc.denton@unatco.int
param-account=jc.denton@unatco.int
Enabled=1
ConnectAutomatically=1
AutomaticPresenceType=2
AutomaticPresenceStatus=available
AutomaticPresenceMessage=My vision is augmented
Nickname=JC
AvatarMime=image/jpeg

[%s]
manager=fakecm
protocol=fakeprotocol
DisplayName=Personal account
NormalizedName=jc.denton@example.com
param-account=jc.denton@example.com
Enabled=1
ConnectAutomatically=0
AutomaticPresenceType=2
AutomaticPresenceStatus=available
AutomaticPresenceMessage=My vision is augmented
Nickname=JC
AvatarMime=image/jpeg
""" % (account1_id, account2_id))
    accounts_cfg.close()

    os.makedirs(accounts_dir + '/' + account1_id)
    avatar_bin = open(accounts_dir + '/' + account1_id + '/avatar.bin', 'w')
    avatar_bin.write('Deus Ex')
    avatar_bin.close()

    os.makedirs(accounts_dir + '/' + account2_id)
    avatar_bin = open(accounts_dir + '/' + account2_id + '/avatar.bin', 'w')
    avatar_bin.write('Invisible War')
    avatar_bin.close()

    account_connections_file = open(accounts_dir + '/.mc_connections', 'w')
    account_connections_file.write("")
    account_connections_file.close()

def test(q, bus, unused):
    # make sure RequestConnection doesn't get called yet
    events = [EventPattern('dbus-method-call', method='RequestConnection')]
    q.forbid_events(events)

    # Wait for MC to load
    mc = make_mc(bus, q.append)

    q.expect_many(
            EventPattern('dbus-signal', signal='NameOwnerChanged',
                predicate=lambda e: e.args[0] == cs.AM),
            EventPattern('dbus-signal', signal='NameOwnerChanged',
                predicate=lambda e: e.args[0] == cs.CD),
            )

    # Trying to make a channel on account 1 doesn't work, because it's
    # not valid

    account_path = (cs.tp_path_prefix + '/Account/' + account1_id)

    cd = bus.get_object(cs.CD, cs.CD_PATH)

    user_action_time = dbus.Int64(1238582606)
    request = dbus.Dictionary({
            cs.CHANNEL + '.ChannelType': cs.CHANNEL_TYPE_TEXT,
            cs.CHANNEL + '.TargetHandleType': cs.HT_CONTACT,
            cs.CHANNEL + '.TargetID': 'juliet',
            }, signature='sv')
    call_async(q, cd, 'CreateChannel',
            account_path, request, user_action_time, "",
            dbus_interface=cs.CD)
    ret = q.expect('dbus-return', method='CreateChannel')
    request_path = ret.value[0]

    cr = bus.get_object(cs.CD, request_path)
    request_props = cr.GetAll(cs.CR, dbus_interface=cs.PROPERTIES_IFACE)
    assert request_props['Account'] == account_path
    assert request_props['Requests'] == [request]
    assert request_props['UserActionTime'] == user_action_time
    assert request_props['PreferredHandler'] == ""
    assert request_props['Interfaces'] == []

    sync_dbus(bus, q, mc)

    cr.Proceed(dbus_interface=cs.CR)

    # FIXME: error isn't specified (NotAvailable perhaps?)
    q.expect('dbus-signal', path=cr.object_path,
            interface=cs.CR, signal='Failed')

    # Make account 1 valid: it should connect automatically

    account_path = (cs.tp_path_prefix + '/Account/' + account1_id)
    account = bus.get_object(cs.MC, account_path)

    sync_dbus(bus, q, mc)
    q.unforbid_events(events)

    call_async(q, account, 'UpdateParameters', {'password': 'nanotech'}, [],
            dbus_interface=cs.ACCOUNT)

    expected_params = {'password': 'nanotech',
            'account': 'jc.denton@unatco.int'}

    e = q.expect('dbus-method-call', method='RequestConnection',
            args=['fakeprotocol', expected_params],
            destination=cs.tp_name_prefix + '.ConnectionManager.fakecm',
            path=cs.tp_path_prefix + '/ConnectionManager/fakecm',
            interface=cs.tp_name_prefix + '.ConnectionManager',
            handled=False)

    conn = SimulatedConnection(q, bus, 'fakecm', 'fakeprotocol', '_',
            'myself', has_presence=True)

    q.dbus_return(e.message, conn.bus_name, conn.object_path, signature='so')

    q.expect('dbus-method-call', method='Connect',
            path=conn.object_path, handled=True, interface=cs.CONN)
    conn.StatusChanged(cs.CONN_STATUS_CONNECTED, cs.CONN_STATUS_REASON_NONE)

    set_presence = q.expect('dbus-method-call', path=conn.object_path,
            interface=cs.CONN_IFACE_SIMPLE_PRESENCE, method='SetPresence',
            handled=True)

    e = q.expect('dbus-signal', signal='AccountPropertyChanged',
            path=account_path, interface=cs.ACCOUNT,
            predicate=lambda e: 'CurrentPresence' in e.args[0]
                and e.args[0]['CurrentPresence'][2] != '')

    assert e.args[0]['CurrentPresence'] == (cs.PRESENCE_TYPE_AVAILABLE,
            'available', 'My vision is augmented')

    # Request an online presence on account 2, then make it valid

    q.forbid_events(events)

    account_path = (cs.tp_path_prefix + '/Account/' + account2_id)
    account = bus.get_object(cs.MC, account_path)

    requested_presence = dbus.Struct((dbus.UInt32(cs.PRESENCE_TYPE_BUSY),
        'busy', 'Talking to Illuminati'))
    account.Set(cs.ACCOUNT, 'RequestedPresence',
            dbus.Struct(requested_presence, variant_level=1),
            dbus_interface=cs.PROPERTIES_IFACE)

    sync_dbus(bus, q, mc)
    q.unforbid_events(events)

    # Make the account valid
    call_async(q, account, 'UpdateParameters', {'password': 'nanotech'}, [],
            dbus_interface=cs.ACCOUNT)

    expected_params = {'password': 'nanotech',
            'account': 'jc.denton@example.com'}

    e = q.expect('dbus-method-call', method='RequestConnection',
            args=['fakeprotocol', expected_params],
            destination=cs.tp_name_prefix + '.ConnectionManager.fakecm',
            path=cs.tp_path_prefix + '/ConnectionManager/fakecm',
            interface=cs.tp_name_prefix + '.ConnectionManager',
            handled=False)

    conn = SimulatedConnection(q, bus, 'fakecm', 'fakeprotocol', '_',
            'myself', has_presence=True)

    q.dbus_return(e.message, conn.bus_name, conn.object_path, signature='so')

    q.expect('dbus-method-call', method='Connect',
            path=conn.object_path, handled=True, interface=cs.CONN)
    conn.StatusChanged(cs.CONN_STATUS_CONNECTED, cs.CONN_STATUS_REASON_NONE)

    set_presence = q.expect('dbus-method-call', path=conn.object_path,
            interface=cs.CONN_IFACE_SIMPLE_PRESENCE, method='SetPresence',
            handled=True)

    e = q.expect('dbus-signal', signal='AccountPropertyChanged',
            path=account_path, interface=cs.ACCOUNT,
            predicate=lambda e: 'CurrentPresence' in e.args[0]
                and e.args[0]['CurrentPresence'][1] == 'busy')

    assert e.args[0]['CurrentPresence'] == (cs.PRESENCE_TYPE_BUSY,
            'busy', 'Talking to Illuminati')

if __name__ == '__main__':
    preseed()
    exec_test(test, {})