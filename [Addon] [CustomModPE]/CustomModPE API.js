// CustomMobPE Object
var CustomModPE = {};

// Teleport Library
CustomModPE.MovePlayer = {};
// PrivateMessage Library
CustomModPE.PrivateMessage = {};
// PrivateMessage Types Library
CustomModPE.PrivateMessage.MessageTypes = {};
// SetGameMode Library
CustomModPE.SetGameMode = {};
// Disconnect Library
CustomModPE.Disconnect = {};

// Teleport to other place
CustomModPE.MovePlayer.send = function (entityId, x, y, z) {
    clientMessage("@TELEPORT");
    Entity.setPosition(entityId, x, y, z);
};

// Send PrivateMessage
CustomModPE.PrivateMessage.send = function (entityId, message, message_type) {
    if (Player.isPlayer(entityId)) {
        if (message_type === undefined)
            message_type = '0';
        else if (message_type.constructor === Number)
            message_type = message_type.toString();
        var cache = Player.getName(entityId);
        Entity.setNameTag(entityId, "@PRIVATE_SENDCHAT" + message_type + message);
        Entity.setNameTag(entityId, cache);
    }
};

// PrivateMessage Types
CustomModPE.PrivateMessage.MessageTypes.TYPE_RAW = 0;
CustomModPE.PrivateMessage.MessageTypes.TYPE_CHAT = 1;
CustomModPE.PrivateMessage.MessageTypes.TYPE_TRANSLATION = 2;
CustomModPE.PrivateMessage.MessageTypes.TYPE_POPUP = 3;
CustomModPE.PrivateMessage.MessageTypes.TYPE_TIP = 4;
CustomModPE.PrivateMessage.MessageTypes.TYPE_SYSTEM = 5;

// Change Player's gametype
CustomModPE.SetGameMode.send = function (entityId, gametype) {
    if (Player.isPlayer(entityId)) {
        if (gametype === undefined)
            return;
        if (gametype.constructor === Number)
            gametype = gametype.toString();
        var cache = Player.getName(entityId);
        Entity.setNameTag(entityId, "@GAMEMODE" + gametype);
        Entity.setNameTag(entityId, cache);
    }
};

// Disconnect Player from server
CustomModPE.Disconnect.send = function (entityId, message) {
    if (Player.isPlayer(entityId)) {
        if (message === undefined)
            message = "";
        var cache = Player.getName(entityId);
        Entity.setNameTag(entityId, "@DISCONNECT" + message);
        Entity.setNameTag(entityId, cache);
    }
};

// Synchronize with other scripts
function newLevel() {
    var scripts = net.zhuoweizhang.mcpelauncher.ScriptManager.scripts;
    var ScriptableObject = org.mozilla.javascript.ScriptableObject;

    var loaded = false;

    for (var i = 0; i < scripts.size() ; i++) {
        var scope = scripts.get(i).scope;
        if (!ScriptableObject.hasProperty(scope, "CustomModPE")) {
            ScriptableObject.putProperty(scope, "CustomModPE", CustomModPE);
            loaded = true;
        }
    }
}