"""
Some handy constants for other tests to share and enjoy.
"""

from dbus import PROPERTIES_IFACE, INTROSPECTABLE_IFACE
from servicetest import tp_name_prefix, tp_path_prefix

HT_CONTACT = 1
HT_ROOM = 2

CHANNEL = tp_name_prefix + ".Channel"
CHANNEL_IFACE_GROUP = CHANNEL + ".Interface.Group"
CHANNEL_IFACE_HOLD = CHANNEL + ".Interface.Hold"
CHANNEL_IFACE_MEDIA_SIGNALLING = CHANNEL + ".Interface.MediaSignalling"
CHANNEL_TYPE_TEXT = CHANNEL + ".Type.Text"
CHANNEL_TYPE_TUBES = CHANNEL + ".Type.Tubes"
CHANNEL_IFACE_TUBE = CHANNEL + ".Interface.Tube.DRAFT"
CHANNEL_TYPE_STREAM_TUBE = CHANNEL + ".Type.StreamTube.DRAFT"
CHANNEL_TYPE_DBUS_TUBE = CHANNEL + ".Type.DBusTube.DRAFT"
CHANNEL_TYPE_STREAMED_MEDIA = CHANNEL + ".Type.StreamedMedia"
CHANNEL_TYPE_TEXT = CHANNEL + ".Type.Text"

TP_AWKWARD_PROPERTIES = tp_name_prefix + ".Properties"
PROPERTY_FLAG_READ = 1
PROPERTY_FLAG_WRITE = 2

CHANNEL_TYPE = CHANNEL + '.ChannelType'
TARGET_HANDLE_TYPE = CHANNEL + '.TargetHandleType'
TARGET_HANDLE = CHANNEL + '.TargetHandle'
TARGET_ID = CHANNEL + '.TargetID'
REQUESTED = CHANNEL + '.Requested'
INITIATOR_HANDLE = CHANNEL + '.InitiatorHandle'
INITIATOR_ID = CHANNEL + '.InitiatorID'
INTERFACES = CHANNEL + '.Interfaces'

CONN = tp_name_prefix + ".Connection"
CONN_IFACE_CONTACTS = CONN + '.Interface.Contacts'
CONN_IFACE_CONTACT_CAPA = CONN + '.Interface.ContactCapabilities.DRAFT'
CONN_IFACE_REQUESTS = CONN + '.Interface.Requests'

CONN_STATUS_CONNECTED = 0
CONN_STATUS_CONNECTING = 1
CONN_STATUS_DISCONNECTED = 2

CONN_STATUS_REASON_NONE = 0
CONN_STATUS_REASON_REQUESTED = 1
CONN_STATUS_REASON_NETWORK_ERROR = 2

PRESENCE_TYPE_UNSET = 0
PRESENCE_TYPE_OFFLINE = 1
PRESENCE_TYPE_AVAILABLE = 2
PRESENCE_TYPE_AWAY = 3
PRESENCE_TYPE_XA = 4
PRESENCE_TYPE_HIDDEN = 5
PRESENCE_TYPE_BUSY = 6
PRESENCE_TYPE_UNKNOWN = 7
PRESENCE_TYPE_ERROR = 8

ERROR = tp_name_prefix + '.Error'
INVALID_ARGUMENT = ERROR + '.InvalidArgument'
INVALID_HANDLE = ERROR + '.InvalidHandle'
NOT_IMPLEMENTED = ERROR + '.NotImplemented'
NOT_AVAILABLE = ERROR + '.NotAvailable'
PERMISSION_DENIED = ERROR + '.PermissionDenied'
CANCELLED = ERROR + '.Cancelled'

TUBE_PARAMETERS = CHANNEL_IFACE_TUBE + '.Parameters'
TUBE_STATE = CHANNEL_IFACE_TUBE + '.State'
STREAM_TUBE_SERVICE = CHANNEL_TYPE_STREAM_TUBE + '.Service'
DBUS_TUBE_SERVICE_NAME = CHANNEL_TYPE_DBUS_TUBE + '.ServiceName'
DBUS_TUBE_DBUS_NAMES = CHANNEL_TYPE_DBUS_TUBE + '.DBusNames'

TUBE_CHANNEL_STATE_LOCAL_PENDING = 0
TUBE_CHANNEL_STATE_REMOTE_PENDING = 1
TUBE_CHANNEL_STATE_OPEN = 2
TUBE_CHANNEL_STATE_NOT_OFFERED = 3

MEDIA_STREAM_TYPE_AUDIO = 0
MEDIA_STREAM_TYPE_VIDEO = 1

SOCKET_ADDRESS_TYPE_UNIX = 0
SOCKET_ADDRESS_TYPE_ABSTRACT_UNIX = 1
SOCKET_ADDRESS_TYPE_IPV4 = 2
SOCKET_ADDRESS_TYPE_IPV6 = 3

SOCKET_ACCESS_CONTROL_LOCALHOST = 0
SOCKET_ACCESS_CONTROL_PORT = 1
SOCKET_ACCESS_CONTROL_NETMASK = 2
SOCKET_ACCESS_CONTROL_CREDENTIALS = 3

TUBE_STATE_LOCAL_PENDING = 0
TUBE_STATE_REMOTE_PENDING = 1
TUBE_STATE_OPEN = 2
TUBE_STATE_NOT_OFFERED = 3

TUBE_TYPE_DBUS = 0
TUBE_TYPE_STREAM = 1

MEDIA_STREAM_DIRECTION_NONE = 0
MEDIA_STREAM_DIRECTION_SEND = 1
MEDIA_STREAM_DIRECTION_RECEIVE = 2
MEDIA_STREAM_DIRECTION_BIDIRECTIONAL = 3

MEDIA_STREAM_PENDING_LOCAL_SEND = 1
MEDIA_STREAM_PENDING_REMOTE_SEND = 2

MEDIA_STREAM_TYPE_AUDIO = 0
MEDIA_STREAM_TYPE_VIDEO = 1

MEDIA_STREAM_STATE_DISCONNECTED = 0
MEDIA_STREAM_STATE_CONNECTING = 1
MEDIA_STREAM_STATE_CONNECTED = 2

MEDIA_STREAM_DIRECTION_NONE = 0
MEDIA_STREAM_DIRECTION_SEND = 1
MEDIA_STREAM_DIRECTION_RECEIVE = 2
MEDIA_STREAM_DIRECTION_BIDIRECTIONAL = 3

CLIENT = tp_name_prefix + '.Client.DRAFT'
CLIENT_PATH = tp_path_prefix + '/Client'
OBSERVER = tp_name_prefix + '.Client.Observer.DRAFT'
APPROVER = tp_name_prefix + '.Client.Approver.DRAFT'
HANDLER = tp_name_prefix + '.Client.Handler.DRAFT'

ACCOUNT = tp_name_prefix + '.Account'
ACCOUNT_IFACE_AVATAR = ACCOUNT + '.Interface.Avatar'
ACCOUNT_IFACE_NOKIA_COMPAT = ACCOUNT + '.Interface.Compat'
ACCOUNT_IFACE_NOKIA_REQUESTS = ACCOUNT + '.Interface.ChannelRequests'
ACCOUNT_IFACE_NOKIA_CONDITIONS = 'com.nokia.Account.Interface.Conditions'

AM = tp_name_prefix + '.AccountManager'
AM_IFACE_CREATION_DRAFT = AM + '.Interface.Creation.DRAFT'
AM_IFACE_NOKIA_QUERY = 'com.nokia.AccountManager.Interface.Query'
AM_PATH = tp_path_prefix + '/AccountManager'

CR = tp_name_prefix + '.ChannelRequest'
CDO = tp_name_prefix + '.ChannelDispatchOperation.DRAFT'

CD = tp_name_prefix + '.ChannelDispatcher.DRAFT'
CD_BUS_NAME = tp_name_prefix + '.ChannelDispatcher'
CD_IFACE_OP_LIST = tp_name_prefix + '.ChannelDispatcher.Interface.OperationList.DRAFT'
CD_PATH = tp_path_prefix + '/ChannelDispatcher'
