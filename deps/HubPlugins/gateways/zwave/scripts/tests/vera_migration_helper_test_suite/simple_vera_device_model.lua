return {
    AutomationDevices = 0,
    BlacklistedPlugins = {},
    BuildVersion = "*1.7.4971*",
    City_description = "Kharkiv",
    Country_description = "Ukraine",
    DataVersion = 669169038,
    DataVersion_Changed = "1",
    DataVersion_Static = "107",
    DeviceSync = "1628669242",
    Device_Num_Next = "59",
    EnableSSL = "0",
    EnableUPnP = "0",
    ExtraLuaFiles = {},
    InstalledPlugins = {},
    InstalledPlugins2 = {
        {
            AllowMultiple = "1",
            Approved = "1",
            AutoUpdate = "1",
            Files = {
                {
                    Compress = "1",
                    DestName = "S_SercommIPCamera.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "S",
                    SourceName = "S_SercommIPCamera.xml"
                },
                {
                    Compress = "1",
                    DestName = "D_SercommIPCamera.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "D",
                    SourceName = "D_SercommIPCamera.xml"
                },
                {
                    Compress = "1",
                    DestName = "sercomm_configuration_mios.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "sercomm_configuration_mios.lua"
                },
                {
                    Compress = "1",
                    DestName = "I_SercommIPCamera.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "I",
                    SourceName = "I_SercommIPCamera.xml"
                },
                {
                    Compress = "1",
                    DestName = "L_SercommIPCamera.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "L_SercommIPCamera.lua"
                },
                {
                    Compress = "1",
                    DestName = "L_SercommIPCameraManager.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "L",
                    SourceName = "L_SercommIPCameraManager.lua"
                },
                {
                    Compress = "1",
                    DestName = "D_MotionSensor_Camera.json",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "J",
                    SourceName = "D_MotionSensor_Camera.json"
                }
            },
            Hidden = "0",
            Icon = "plugins/icons/6196.png",
            Instructions = "http://wiki.mios.com/index.php/Main_Page",
            Lua = {
                {
                    FileName = "L_SercommIPCameraManager.lua"
                }
            },
            OldTimestamp = 1586246023,
            OldVersion = "38174",
            RemoteFriendly = 1,
            SystemPlugin = 1,
            Title = "Sercomm IP Camera",
            Version = "39974",
            VersionMajor = "4",
            VersionMinor = "32",
            id = 6196,
            timestamp = 1610549430
        },
        {
            AllowMultiple = "0",
            Approved = "1",
            AutoUpdate = "1",
            Files = {
                {
                    Compress = "1",
                    DestName = "L_AmazonAlexaHelper.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "L",
                    SourceName = "L_AmazonAlexaHelper.lua"
                }
            },
            Hidden = "0",
            Icon = "images/plugin.png",
            Instructions = "http://wiki.mios.com/index.php/Main_Page",
            Lua = {
                {
                    FileName = "L_AmazonAlexaHelper.lua"
                }
            },
            RemoteFriendly = 1,
            SystemPlugin = 1,
            Title = "Amazon Alexa Helper",
            Version = "38956",
            VersionMajor = "1",
            VersionMinor = "38",
            id = 8840,
            timestamp = 1586246024
        },
        {
            AllowMultiple = "0",
            Approved = "1",
            AutoUpdate = "1",
            Devices = {
                {
                    DeviceFileName = "D_G550Siren.xml",
                    DeviceType = "urn:schemas-micasaverde-com:device:G550Siren:1",
                    ImplFile = "I_G550Siren.xml",
                    Invisible = "0"
                }
            },
            Files = {
                {
                    Compress = "1",
                    DestName = "siren_configuration.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "siren_configuration.lua"
                },
                {
                    Compress = "1",
                    DestName = "I_G550Siren.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "I",
                    SourceName = "I_G550Siren.xml"
                },
                {
                    Compress = "1",
                    DestName = "L_G550Siren.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "L_G550Siren.lua"
                },
                {
                    Compress = "1",
                    DestName = "D_G550Siren.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "D",
                    SourceName = "D_G550Siren.xml"
                },
                {
                    Compress = "1",
                    DestName = "S_G550Siren.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "S",
                    SourceName = "S_G550Siren.xml"
                },
                {
                    Compress = "1",
                    DestName = "D_G550Siren.json",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "J",
                    SourceName = "D_G550Siren.json"
                }
            },
            Hidden = "0",
            Icon = "plugins/icons/8900.png",
            Instructions = "http://wiki.mios.com/index.php/Main_Page",
            OldTimestamp = 1586246025,
            OldVersion = "38306",
            RemoteFriendly = 1,
            SystemPlugin = 0,
            Title = "VeraSecure",
            Version = "40118",
            VersionMajor = "2",
            VersionMinor = "37",
            id = 8900,
            timestamp = 1610549433
        },
        {
            AllowMultiple = "0",
            Approved = "1",
            AutoUpdate = "1",
            Devices = {
                {
                    DeviceFileName = "D_SmartHomeHelper.xml",
                    DeviceType = "urn:schemas-upnp-org:device:GoogleHome:1",
                    ImplFile = "",
                    Invisible = "1"
                }
            },
            Files = {
                {
                    Compress = "1",
                    DestName = "L_SmartHomeHelper.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "L",
                    SourceName = "L_SmartHomeHelper.lua"
                },
                {
                    Compress = "1",
                    DestName = "D_SmartHomeHelper.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "D",
                    SourceName = "D_SmartHomeHelper.xml"
                },
                {
                    Compress = "1",
                    DestName = "I_SmartHomeHelper.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "I",
                    SourceName = "I_SmartHomeHelper.xml"
                }
            },
            Hidden = "0",
            Icon = "plugins/icons/9092.png",
            Instructions = "https://getvera.com/",
            Lua = {
                {
                    FileName = "L_SmartHomeHelper.lua"
                }
            },
            OldTimestamp = 1586246026,
            OldVersion = "38816",
            RemoteFriendly = 1,
            SystemPlugin = 1,
            Title = "Google Home",
            Version = "39041",
            VersionMajor = "1",
            VersionMinor = "1",
            id = 9092,
            timestamp = 1587542163
        },
        {
            AllowMultiple = "1",
            Approved = "1",
            AutoUpdate = "1",
            Files = {
                {
                    Compress = "1",
                    DestName = "ezviz_video_uploader_mms.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "ezviz_video_uploader_mms.lua"
                },
                {
                    Compress = "1",
                    DestName = "ezviz_video_uploader_ezlo.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "ezviz_video_uploader_ezlo.lua"
                },
                {
                    Compress = "1",
                    DestName = "D_DoorbellCamera.json",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "J",
                    SourceName = "D_DoorbellCamera.json"
                },
                {
                    Compress = "1",
                    DestName = "ezviz_crypto.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "ezviz_crypto.lua"
                },
                {
                    Compress = "1",
                    DestName = "ezviz_config_loader.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "ezviz_config_loader.lua"
                },
                {
                    Compress = "1",
                    DestName = "L_EzvizUtils.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "L_EzvizUtils.lua"
                },
                {
                    Compress = "1",
                    DestName = "ezviz_logger.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "ezviz_logger.lua"
                },
                {
                    Compress = "1",
                    DestName = "ezviz_http.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "ezviz_http.lua"
                },
                {
                    Compress = "1",
                    DestName = "ezviz_video_manager.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "ezviz_video_manager.lua"
                },
                {
                    Compress = "1",
                    DestName = "L_EzvizGlobal.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "L",
                    SourceName = "L_EzvizGlobal.lua"
                },
                {
                    Compress = "1",
                    DestName = "ezviz_event_listener.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "ezviz_event_listener.lua"
                },
                {
                    Compress = "1",
                    DestName = "S_EzvizCamera.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "S",
                    SourceName = "S_EzvizCamera.xml"
                },
                {
                    Compress = "1",
                    DestName = "L_EzvizLogger.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "L_EzvizLogger.lua"
                },
                {
                    Compress = "1",
                    DestName = "ezviz_config.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "ezviz_config.lua"
                },
                {
                    Compress = "1",
                    DestName = "L_EzvizCamera.lua",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "L_EzvizCamera.lua"
                },
                {
                    Compress = "1",
                    DestName = "I_EzvizCamera.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "I_EzvizCamera.xml"
                },
                {
                    Compress = "1",
                    DestName = "D_EzvizCamera.xml",
                    DestPath = "",
                    Encrypt = "0",
                    Role = "M",
                    SourceName = "D_EzvizCamera.xml"
                }
            },
            Hidden = "0",
            Icon = "images/plugin.png",
            Instructions = "https://getvera.com/products/vistacam-702",
            Lua = {
                {
                    FileName = "L_EzvizGlobal.lua"
                }
            },
            OldTimestamp = 1610549435,
            OldVersion = "40568",
            RemoteFriendly = 1,
            SystemPlugin = 1,
            Title = "Ezlo Cameras",
            Version = "40698",
            VersionMajor = "1",
            VersionMinor = "58",
            id = 9284,
            timestamp = 1627287382
        }
    },
    LoadTime = 1628669169,
    LuaUPnPAlive = "1628669238",
    Mode = "1",
    ModeSetting = "1:DC*;2:DC*;3:DC*;4:DC*",
    PK_AccessPoint = "55100259",
    PluginsSynced = "1",
    PluginsSyncedMMS = "1",
    RA_Server = "vera-eu-oem-relay31.mios.com",
    RA_Server_Back = "vera-eu-oem-relay11.mios.com",
    Region_description = "Kharkiv",
    Scene_Num_Next = "1",
    ServerBackup = "1",
    Server_Device = "vera-us-oem-device12.mios.com",
    Server_Device_Alt = "vera-us-oem-device11.mios.com",
    ShortVersion = "7.31",
    SvnVersion = "*167978e*",
    TemperatureFormat = "F",
    UnassignedDevices = 0,
    UpnpDiscoveryInterval = "600",
    UpnpScanning = "0",
    Using_2G = 0,
    breach_delay = 30,
    country_pk = "804",
    device_sync = "",
    devices = {
        {
            ControlURLs = {
                service_1 = {
                    ControlURL = "/upnp/control/dev_1",
                    EventURL = "/upnp/event/dev_1",
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    serviceType = "urn:schemas-micasaverde-org:service:ZWaveNetwork:1"
                }
            },
            category_num = 19,
            device_file = "D_ZWaveNetwork.xml",
            device_type = "urn:schemas-micasaverde-com:device:ZWaveNetwork:1",
            id = 1,
            invisible = "1",
            local_udn = "uuid:4d494342-5342-5645-0001-00000348c363",
            name = "ZWave",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "1",
                    variable = "Use45"
                },
                {
                    id = 1,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "0",
                    variable = "UseMR"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "0",
                    variable = "LimitNeighbors"
                },
                {
                    id = 3,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "1628669169",
                    variable = "LastUpdate"
                },
                {
                    id = 4,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1",
                    variable = "AutoConfigure"
                },
                {
                    id = 5,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "2",
                    variable = "NetStatusID"
                },
                {
                    id = 6,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "Resetting ZWave Network",
                    variable = "NetStatusText"
                },
                {
                    id = 7,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "1",
                    variable = "LockComPort"
                },
                {
                    id = 8,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "/dev/ttyS0",
                    variable = "ComPort"
                },
                {
                    id = 9,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "0",
                    variable = "CommFailure"
                },
                {
                    id = 10,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "0",
                    variable = "CommFailureTime"
                },
                {
                    id = 11,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "1",
                    variable = "PollingEnabled"
                },
                {
                    id = 12,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "20",
                    variable = "PollDelayInitial"
                },
                {
                    id = 13,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "10",
                    variable = "PollDelayDeadTime"
                },
                {
                    id = 14,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "30",
                    variable = "PollMinDelay"
                },
                {
                    id = 15,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "60",
                    variable = "PollFrequency"
                },
                {
                    id = 16,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "6.01 L:1",
                    variable = "VersionInfo"
                },
                {
                    id = 17,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "House: 0xfd217d71 Node 1 Suc 0",
                    variable = "HomeID"
                },
                {
                    id = 18,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "Master SIS:NO PRI:YES",
                    variable = "Role"
                },
                {
                    id = 19,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1586245979,0",
                    variable = "CommFailureAlarm"
                },
                {
                    id = 20,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "",
                    variable = "LastDongleBackup"
                },
                {
                    id = 21,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "-11",
                    variable = "LastTimeOffset"
                },
                {
                    id = 22,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "Poll failed",
                    variable = "LastError"
                }
            },
            subcategory_num = 0,
            time_created = 1586245974
        },
        {
            ControlURLs = {
                service_2 = {
                    ControlURL = "/upnp/control/dev_2",
                    EventURL = "/upnp/event/dev_2",
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    serviceType = "urn:schemas-micasaverde-org:service:ZigbeeNetwork:1"
                }
            },
            altid = "",
            device_file = "D_ZigbeeNetwork.xml",
            device_json = "",
            device_type = "urn:schemas-micasaverde-com:device:ZigbeeNetwork:1",
            disabled = 0,
            embedded = 0,
            id = 2,
            id_parent = 0,
            impl_file = "",
            invisible = "1",
            ip = "",
            local_udn = "uuid:4d494342-5342-5645-0002-00000348c363",
            mac = "000D6F000F08162D",
            manufacturer = "",
            model = "",
            name = "Zigbee Network",
            room = "0",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "1",
                    variable = "LockComPort"
                },
                {
                    id = 1,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "/dev/ttyXRUSB1",
                    variable = "ComPort"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "3",
                    variable = "NetStatusID"
                },
                {
                    id = 3,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "GET_LANG(resetting_zigbee_network,Resetting Zigbee Network)",
                    variable = "NetStatusText"
                },
                {
                    id = 4,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "5.0.0 GA build 95",
                    variable = "StackVersion"
                },
                {
                    id = 5,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "1",
                    variable = "RediscoverEnabled"
                },
                {
                    id = 6,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "60",
                    variable = "RediscoverInitialDelay"
                },
                {
                    id = 7,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "120",
                    variable = "RediscoverMinDelay"
                },
                {
                    id = 8,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "120",
                    variable = "RediscoverFreq"
                },
                {
                    id = 9,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "1",
                    variable = "PollingEnabled"
                },
                {
                    id = 10,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "20",
                    variable = "PollDelayInitial"
                },
                {
                    id = 11,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "60",
                    variable = "PollMinDelay"
                },
                {
                    id = 12,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "60",
                    variable = "PollFrequency"
                },
                {
                    id = 13,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "",
                    variable = "PendingRemoveDevices"
                },
                {
                    id = 14,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "",
                    variable = "UnknownDevices"
                },
                {
                    id = 15,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "c3b3",
                    variable = "ZigbeePanID"
                },
                {
                    id = 16,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "3413f561018ceaed",
                    variable = "ZigbeeEPanID"
                },
                {
                    id = 17,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "15",
                    variable = "ZigbeeChannel"
                }
            },
            time_created = "1586245975"
        },
        {
            ControlURLs = {
                service_3 = {
                    ControlURL = "/upnp/control/dev_3",
                    EventURL = "/upnp/event/dev_3",
                    service = "urn:micasaverde-com:serviceId:BluetoothNetwork1",
                    serviceType = "urn:schemas-micasaverde-org:service:BluetoothNetwork:1"
                }
            },
            altid = "",
            device_file = "D_BluetoothNetwork.xml",
            device_json = "",
            device_type = "urn:schemas-micasaverde-com:device:BluetoothNetwork:1",
            disabled = 0,
            embedded = 0,
            id = 3,
            id_parent = 0,
            impl_file = "",
            ip = "",
            local_udn = "uuid:4d494342-5342-5645-0003-00000348c363",
            mac = "",
            manufacturer = "",
            model = "",
            name = "Bluetooth Network",
            room = "0",
            states = {},
            time_created = "1586245980"
        },
        {
            ControlURLs = {
                service_4 = {
                    ControlURL = "/upnp/control/dev_4",
                    EventURL = "/upnp/event/dev_4",
                    service = "urn:micasaverde-com:serviceId:LPRFNetwork1",
                    serviceType = "urn:schemas-micasaverde-org:service:LPRFNetwork:1"
                }
            },
            altid = "",
            device_file = "D_LPRFNetwork.xml",
            device_json = "",
            device_type = "urn:schemas-micasaverde-com:device:LPRFNetwork:1",
            disabled = 0,
            embedded = 0,
            id = 4,
            id_parent = 0,
            impl_file = "",
            invisible = "1",
            ip = "",
            local_udn = "uuid:4d494342-5342-5645-0004-00000348c363",
            mac = "",
            manufacturer = "",
            model = "",
            name = "2GIG Low Power RF Network",
            room = "0",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:LPRFNetwork1",
                    value = "/dev/ttyXRUSB0",
                    variable = "ComPort"
                },
                {
                    id = 1,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1",
                    variable = "CommFailure"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1586245991",
                    variable = "CommFailureTime"
                }
            },
            time_created = "1586245986"
        },
        {
            ControlURLs = {
                service_5 = {
                    ControlURL = "/upnp/control/dev_5",
                    EventURL = "/upnp/event/dev_5",
                    service = "urn:micasaverde-com:serviceId:SceneController1",
                    serviceType = "urn:schemas-micasaverde-com:service:SceneController:1"
                },
                service_6 = {
                    ControlURL = "/upnp/control/dev_6",
                    EventURL = "/upnp/event/dev_6",
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    serviceType = "urn:schemas-micasaverde-com:service:HaDevice:1"
                }
            },
            altid = "1",
            category_num = 14,
            device_file = "D_SceneController1.xml",
            device_json = "D_SceneController1.json",
            device_type = "urn:schemas-micasaverde-com:device:SceneController:1",
            disabled = 0,
            embedded = 0,
            id = 5,
            id_parent = 1,
            impl_file = "",
            invisible = "1",
            ip = "",
            local_udn = "uuid:4d494342-5342-5645-0005-00000348c363",
            mac = "",
            manufacturer = "",
            model = "",
            name = "_Scene Controller",
            room = "0",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:SceneController1",
                    value = "",
                    variable = "Scenes"
                },
                {
                    id = 1,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1",
                    variable = "IgnoreRoom"
                }
            },
            subcategory_num = 0,
            time_created = "1586245993"
        },
        {
            ControlURLs = {
                service_10 = {
                    ControlURL = "/upnp/control/dev_10",
                    EventURL = "/upnp/event/dev_10",
                    service = "urn:micasaverde-com:serviceId:SecuritySensor1",
                    serviceType = "urn:schemas-micasaverde-com:service:SecuritySensor:1"
                },
                service_7 = {
                    ControlURL = "/upnp/control/dev_7",
                    EventURL = "/upnp/event/dev_7",
                    service = "urn:upnp-org:serviceId:SwitchPower1",
                    serviceType = "urn:schemas-upnp-org:service:SwitchPower:1"
                },
                service_8 = {
                    ControlURL = "/upnp/control/dev_8",
                    EventURL = "/upnp/event/dev_8",
                    service = "urn:micasaverde-com:G550Siren1",
                    serviceType = "urn:schemas-micasaverde-com:service:G550Siren:1"
                },
                service_9 = {
                    ControlURL = "/upnp/control/dev_9",
                    EventURL = "/upnp/event/dev_9",
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    serviceType = "urn:schemas-micasaverde-com:service:HaDevice:1"
                }
            },
            altid = "",
            category_num = "24",
            device_file = "D_G550Siren.xml",
            device_json = "D_G550Siren.json",
            device_type = "urn:schemas-micasaverde-com:device:G550Siren:1",
            disabled = 0,
            embedded = 0,
            id = 6,
            id_parent = 0,
            impl_file = "I_G550Siren.xml",
            ip = "",
            local_udn = "uuid:4d494342-5342-5645-0006-00000348c363",
            mac = "",
            manufacturer = "",
            model = "",
            name = "VeraSecure",
            plugin = "8900",
            room = "0",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1:A;2:A;3:A;4:A",
                    variable = "ModeSetting"
                },
                {
                    id = 1,
                    service = "urn:micasaverde-com:serviceId:SecuritySensor1",
                    value = "1",
                    variable = "Armed"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "0",
                    variable = "DebugMode"
                },
                {
                    id = 3,
                    service = "urn:micasaverde-com:G550Siren1",
                    value = "1",
                    variable = "Battery_IDX"
                },
                {
                    id = 4,
                    service = "urn:micasaverde-com:G550Siren1",
                    value = "1",
                    variable = "STORAGE_IDX"
                },
                {
                    id = 5,
                    service = "urn:micasaverde-com:G550Siren1",
                    value = "1",
                    variable = "Cellular_IDX"
                },
                {
                    id = 6,
                    service = "urn:micasaverde-com:G550Siren1",
                    value = "0",
                    variable = "BatteryConnection"
                },
                {
                    id = 7,
                    service = "urn:micasaverde-com:G550Siren1",
                    value = "{\"total\":14400,\"available\":0,\"used\":0,\"level\":0,\"intervals\":[{\"start\":0,\"textColor\":\"#FFEE3241\",\"circleColor\":\"#FFEE3241\",\"end\":9},{\"start\":10,\"textColor\":\"#FFF48220\",\"circleColor\":\"#FFF48220\",\"end\":24},{\"start\":25,\"textColor\":\"#FFEABD1E\",\"circleColor\":\"#FFFCC608\",\"end\":49},{\"start\":50,\"textColor\":\"#FF24803E\",\"circleColor\":\"#FF049E4A\",\"end\":100}],\"previous_idx\":1}",
                    variable = "BatteryInfo"
                },
                {
                    id = 8,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "0",
                    variable = "BatteryLevel"
                },
                {
                    id = 9,
                    service = "urn:micasaverde-com:G550Siren1",
                    value = "{\"total\":\"51200\",\"available\":\"39184\",\"used\":\"12016\",\"intervals\":[{\"start\":1,\"textColor\":\"#FF24803E\",\"circleColor\":\"#FF049E4A\",\"end\":78},{\"start\":79,\"textColor\":\"#FFEABD1E\",\"circleColor\":\"#FFFCC608\",\"end\":90},{\"start\":91,\"textColor\":\"#FFF48220\",\"circleColor\":\"#FFF48220\",\"end\":96},{\"start\":97,\"textColor\":\"#FFEE3241\",\"circleColor\":\"#FFEE3241\",\"end\":100}],\"previous_idx\":1}",
                    variable = "StorageInfo"
                },
                {
                    id = 10,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "0",
                    variable = "sl_TamperAlarm"
                },
                {
                    id = 11,
                    service = "urn:micasaverde-com:G550Siren1",
                    value = "{\"used\":0,\"total\":5120,\"intervals\":[{\"start\":1,\"textColor\":\"#FF24803E\",\"circleColor\":\"#FF049E4A\",\"end\":50},{\"start\":51,\"textColor\":\"#FFEABD1E\",\"circleColor\":\"#FFFCC608\",\"end\":75},{\"start\":76,\"textColor\":\"#FFF48220\",\"circleColor\":\"#FFF48220\",\"end\":90},{\"start\":91,\"textColor\":\"#FFEE3241\",\"circleColor\":\"#FFEE3241\",\"end\":110000}],\"previous_idx\":1}",
                    variable = "CelullarInfo"
                }
            },
            time_created = "1586246025"
        },
        {
            altid = "",
            category_num = "666",
            device_file = "D_SmartHomeHelper.xml",
            device_json = "",
            device_type = "urn:schemas-upnp-org:device:GoogleHome:1",
            disabled = 0,
            embedded = 0,
            id = 7,
            id_parent = 0,
            impl_file = "I_SmartHomeHelper.xml",
            invisible = "1",
            ip = "",
            local_udn = "uuid:4d494342-5342-5645-0007-00000348c363",
            mac = "",
            manufacturer = "",
            model = "",
            name = "Google",
            plugin = "9092",
            room = "0",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:HomeAutomationGateway1",
                    value = "[]",
                    variable = "lastNamesOfSubscribedDevices"
                }
            },
            time_created = "1586246026"
        },
        {
            ControlURLs = {
                service_12 = {
                    ControlURL = "/upnp/control/dev_12",
                    EventURL = "/upnp/event/dev_12",
                    service = "urn:upnp-org:serviceId:SwitchPower1",
                    serviceType = "urn:schemas-upnp-org:service:SwitchPower:1"
                },
                service_13 = {
                    ControlURL = "/upnp/control/dev_13",
                    EventURL = "/upnp/event/dev_13",
                    service = "urn:micasaverde-com:serviceId:EnergyMetering1",
                    serviceType = "urn:schemas-micasaverde-com:service:EnergyMetering:1"
                },
                service_14 = {
                    ControlURL = "/upnp/control/dev_14",
                    EventURL = "/upnp/event/dev_14",
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    serviceType = "urn:schemas-micasaverde-com:service:HaDevice:1"
                }
            },
            altid = "7c87",
            category_num = "3",
            device_file = "D_BinaryLight1.xml",
            device_json = "D_BinaryLight1.json",
            device_type = "urn:schemas-upnp-org:device:BinaryLight:1",
            disabled = 0,
            embedded = 0,
            id = 56,
            id_parent = 2,
            impl_file = "",
            ip = "",
            local_udn = "uuid:4d494342-5342-5645-0038-00000348c363",
            mac = "000D6F0004B91E0F",
            manufacturer = "CentraLite",
            model = "3200-fr",
            name = "Smart Outlet (Type E",
            pnp = "7301",
            poll = {
                "1628669063-1",
                "1628669201-0"
            },
            room = "2",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "13",
                    variable = "LastConfigStep"
                },
                {
                    id = 1,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1",
                    variable = "Configured"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "01",
                    variable = "EndpointList"
                },
                {
                    id = 3,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "01",
                    variable = "HaEndpoint"
                },
                {
                    id = 4,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "0100",
                    variable = "AppTypeId"
                },
                {
                    id = 5,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "0000,0003,0004,0005,0006,0b04,0b05",
                    variable = "SupportedClusters"
                },
                {
                    id = 6,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "0019",
                    variable = "SupportedOutClusters"
                },
                {
                    id = 7,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "1",
                    variable = "PollEnabled"
                },
                {
                    id = 8,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "100",
                    variable = "PowerSourceLevel"
                },
                {
                    id = 9,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "1628669007",
                    variable = "LastWakeup"
                },
                {
                    id = 10,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "01",
                    variable = "BatteryPower"
                },
                {
                    id = 11,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "0000=1,0006=1,0b04=1",
                    variable = "ConfiguredClusters"
                },
                {
                    id = 12,
                    service = "urn:upnp-org:serviceId:SwitchPower1",
                    value = "0",
                    variable = "Status"
                },
                {
                    id = 13,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "00000001",
                    variable = "MeasureType"
                },
                {
                    id = 14,
                    service = "urn:micasaverde-com:serviceId:EnergyMetering1",
                    value = "0.00",
                    variable = "Watts"
                },
                {
                    id = 15,
                    service = "urn:micasaverde-com:serviceId:EnergyMetering1",
                    value = "0,0,0,1628669008,1",
                    variable = "Log"
                },
                {
                    id = 16,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "1",
                    variable = "ActivePowerMultiplier"
                },
                {
                    id = 17,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "1994850314",
                    variable = "ActivePowerDivisor"
                },
                {
                    id = 18,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "0000=1,0006=1,0b04=1",
                    variable = "BindClusters"
                },
                {
                    id = 19,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "0006:1000=1;0b04:2901=0",
                    variable = "ReportAttrib"
                },
                {
                    id = 20,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "0006=1,0b04=1",
                    variable = "ConfigClusters"
                },
                {
                    id = 21,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "1628669008",
                    variable = "LastConfigAttempt"
                },
                {
                    id = 22,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1:;2:;3:;4:",
                    variable = "ModeSetting"
                },
                {
                    id = 23,
                    service = "urn:upnp-org:serviceId:Dimming1",
                    value = "0",
                    variable = "LoadLevelStatus"
                },
                {
                    id = 24,
                    service = "urn:micasaverde-com:serviceId:ZigbeeDevice1",
                    value = "1628669063",
                    variable = "LastPolled"
                },
                {
                    id = 25,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "2.50",
                    variable = "PollRatings"
                },
                {
                    id = 26,
                    service = "urn:micasaverde-com:serviceId:ZigbeeNetwork1",
                    value = "1",
                    variable = "ConsecutivePollFails"
                }
            },
            subcategory_num = "1",
            time_created = "1628669007",
            wakeup = {
                lastKnown = 1628669007,
                list = {
                    1628669023,
                    1628669024,
                    1628669026,
                    1628669039,
                    1628669046,
                    1628669050,
                    1628669051,
                    1628669064,
                    1628669080,
                    1628669082,
                    1628669083,
                    1628669114
                }
            }
        },
        {
            ControlURLs = {
                service_15 = {
                    ControlURL = "/upnp/control/dev_15",
                    EventURL = "/upnp/event/dev_15",
                    service = "urn:upnp-org:serviceId:SwitchPower1",
                    serviceType = "urn:schemas-upnp-org:service:SwitchPower:1"
                },
                service_16 = {
                    ControlURL = "/upnp/control/dev_16",
                    EventURL = "/upnp/event/dev_16",
                    service = "urn:micasaverde-com:serviceId:EnergyMetering1",
                    serviceType = "urn:schemas-micasaverde-com:service:EnergyMetering:1"
                },
                service_17 = {
                    ControlURL = "/upnp/control/dev_17",
                    EventURL = "/upnp/event/dev_17",
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    serviceType = "urn:schemas-micasaverde-com:service:HaDevice:1"
                }
            },
            altid = "2",
            category_num = 3,
            device_file = "D_BinaryLight1.xml",
            device_json = "D_BinaryLight1.json",
            device_type = "urn:schemas-upnp-org:device:BinaryLight:1",
            disabled = 0,
            embedded = 0,
            id = 57,
            id_parent = 1,
            impl_file = "",
            ip = "",
            local_udn = "uuid:4d494342-5342-5645-0039-00000348c363",
            mac = "",
            manufacturer = "",
            model = "",
            name = "plug1",
            room = "3",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "211,156,1,4,16,1,L,R,B,RS,|34,37S,38S:2,43S,44S,50S:4,51S,85,89S,90S,94,108,112S,113S:4,114,115S,117S,122S,129S,133S,134S,152,159,",
                    variable = "Capabilities"
                },
                {
                    id = 1,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "22,55,5e,6c,72,98,9f,",
                    variable = "NodeInfo"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1,",
                    variable = "Neighbors"
                },
                {
                    id = 3,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1628669072",
                    variable = "LastReset"
                },
                {
                    id = 4,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "2",
                    variable = "AssociationNum"
                },
                {
                    id = 5,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "0",
                    variable = "SecurityFailed"
                },
                {
                    id = 6,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "881,3,175",
                    variable = "ManufacturerInfo"
                },
                {
                    id = 7,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "3,6,4,1,2",
                    variable = "VersionInfo"
                },
                {
                    id = 8,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1",
                    variable = "Configured"
                },
                {
                    id = 9,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1",
                    variable = "MeterType"
                },
                {
                    id = 10,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "5",
                    variable = "MeterScale"
                },
                {
                    id = 11,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "0x8,0x9,",
                    variable = "AlarmType"
                },
                {
                    id = 12,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "0,2",
                    variable = "AlarmVersion"
                },
                {
                    id = 13,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = ",0x8,0x9,",
                    variable = "SubscribedAlarms"
                },
                {
                    id = 14,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1,5,0,7,0,7,0",
                    variable = "PlusInfo"
                },
                {
                    id = 15,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "881,943,50907",
                    variable = "FirmwareInfo"
                },
                {
                    id = 16,
                    service = "urn:micasaverde-com:serviceId:Color1",
                    value = "R,G,B",
                    variable = "SupportedColors"
                },
                {
                    id = 17,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "X",
                    variable = "AgiInfo"
                },
                {
                    id = 18,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "",
                    variable = "ConfiguredAssoc"
                },
                {
                    id = 19,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "",
                    variable = "ConfiguredVariable"
                },
                {
                    id = 20,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1:;2:;3:;4:",
                    variable = "ModeSetting"
                },
                {
                    id = 21,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1628669089",
                    variable = "LastUpdate"
                },
                {
                    id = 22,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1628669089",
                    variable = "FirstConfigured"
                },
                {
                    id = 23,
                    service = "urn:upnp-org:serviceId:SwitchPower1",
                    value = "0",
                    variable = "Target"
                },
                {
                    id = 24,
                    service = "urn:upnp-org:serviceId:SwitchPower1",
                    value = "0",
                    variable = "Status"
                },
                {
                    id = 25,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1628669140",
                    variable = "LastRouteUpdate"
                },
                {
                    id = 52,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1",
                    variable = "CommFailure"
                },
                {
                    id = 53,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1628669244",
                    variable = "CommFailureTime"
                }
            },
            subcategory_num = 0,
            time_created = "1628669063"
        },
        {
            ControlURLs = {
                service_18 = {
                    ControlURL = "/upnp/control/dev_18",
                    EventURL = "/upnp/event/dev_18",
                    service = "urn:upnp-org:serviceId:SwitchPower1",
                    serviceType = "urn:schemas-upnp-org:service:SwitchPower:1"
                },
                service_19 = {
                    ControlURL = "/upnp/control/dev_19",
                    EventURL = "/upnp/event/dev_19",
                    service = "urn:micasaverde-com:serviceId:EnergyMetering1",
                    serviceType = "urn:schemas-micasaverde-com:service:EnergyMetering:1"
                },
                service_20 = {
                    ControlURL = "/upnp/control/dev_20",
                    EventURL = "/upnp/event/dev_20",
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    serviceType = "urn:schemas-micasaverde-com:service:HaDevice:1"
                }
            },
            altid = "3",
            category_num = 3,
            device_file = "D_BinaryLight1.xml",
            device_json = "D_BinaryLight1.json",
            device_type = "urn:schemas-upnp-org:device:BinaryLight:1",
            disabled = 0,
            embedded = 0,
            id = 58,
            id_parent = 1,
            impl_file = "",
            ip = "",
            local_udn = "uuid:4d494342-5342-5645-003a-00000348c363",
            mac = "",
            manufacturer = "",
            model = "",
            name = "plug2",
            poll = {
                "1628669166-0"
            },
            room = "4",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "211,156,1,4,16,1,L,R,B,RS,|32,37,39,89,90,94,112,114,115,122,133,134,",
                    variable = "Capabilities"
                },
                {
                    id = 1,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "280,3,2",
                    variable = "ManufacturerInfo"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "3,4,5,2,48",
                    variable = "VersionInfo"
                },
                {
                    id = 3,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1",
                    variable = "Configured"
                },
                {
                    id = 4,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "20,25,27,59,5a,5e,70,72,73,7a,85,86,",
                    variable = "NodeInfo"
                },
                {
                    id = 5,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1,5,0,7,1,7,0",
                    variable = "PlusInfo"
                },
                {
                    id = 6,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "280,770,63841",
                    variable = "FirmwareInfo"
                },
                {
                    id = 7,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "X",
                    variable = "AgiInfo"
                },
                {
                    id = 8,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "",
                    variable = "ConfiguredAssoc"
                },
                {
                    id = 9,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "",
                    variable = "ConfiguredVariable"
                },
                {
                    id = 10,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1:;2:;3:;4:",
                    variable = "ModeSetting"
                },
                {
                    id = 11,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1628669145",
                    variable = "LastUpdate"
                },
                {
                    id = 12,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1628669145",
                    variable = "FirstConfigured"
                },
                {
                    id = 13,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1,2,",
                    variable = "Neighbors"
                },
                {
                    id = 14,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1628669144",
                    variable = "LastReset"
                },
                {
                    id = 15,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1",
                    variable = "AssociationNum"
                },
                {
                    id = 16,
                    service = "urn:upnp-org:serviceId:SwitchPower1",
                    value = "0",
                    variable = "Status"
                },
                {
                    id = 17,
                    service = "urn:micasaverde-com:serviceId:ZWaveNetwork1",
                    value = "1",
                    variable = "ConsecutivePollFails"
                },
                {
                    id = 18,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "0.00",
                    variable = "PollRatings"
                },
                {
                    id = 19,
                    service = "urn:upnp-org:serviceId:SwitchPower1",
                    value = "0",
                    variable = "Target"
                },
                {
                    id = 20,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1628669200",
                    variable = "LastRouteUpdate"
                }
            },
            subcategory_num = 0,
            time_created = "1628669141"
        },
        {
            altid = "4",
            category_num = "4",
            device_file = "D_MotionSensor1.xml",
            device_json = "D_MotionSensorWithTamper1.json",
            device_type = "urn:schemas-micasaverde-com:device:GenericIO:1",
            disabled = 0,
            embedded = 0,
            id = 59,
            id_parent = 1,
            impl_file = "",
            ip = "",
            mac = "",
            manufacturer = "Fibaro",
            model = "FGMS-001 (v3.2)",
            name = "3 in 1 sensor",
            pnp = "4481",
            room = "3",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "0,0,0,4,7,1,|32S,34,48S:1,49:8,86,89,90S,94,112S,113S:5,114,115,122,128,132S:2,133S,134,142S,152,156S,",
                    variable = "Capabilities"
                },
                {
                    id = 1,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "22,31,56,59,5e,72,73,7a,80,86,98,",
                    variable = "NodeInfo"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "",
                    variable = "Neighbors"
                },
                {
                    id = 3,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1628669229",
                    variable = "LastReset"
                },
                {
                    id = 4,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "5",
                    variable = "AssociationNum"
                },
                {
                    id = 5,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "0",
                    variable = "SecurityFailed"
                },
                {
                    id = 6,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "271,2049,4098",
                    variable = "ManufacturerInfo"
                },
                {
                    id = 7,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "3,4,24,3,3",
                    variable = "VersionInfo"
                },
                {
                    id = 8,
                    pnp = "*1.7.4971*",
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1",
                    variable = "EnableFlashControl"
                },
                {
                    id = 9,
                    pnp = "*1.7.4971*",
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1-Motion detection sensitivity (8-255),1d,,6-Motion detection-alarm cancellation delay,2d,,24-Tamper operating modes (0-2),1d,,40-Illuminance report-threshold,2d,,42-Illuminance report-interval,2d,,60-Temperature report-threshold,2d,,62-Temperature measuring-interval,2d,,64-Temperature report-interval,2d,,66-Temperature offset,2d,",
                    variable = "VariablesSet"
                },
                {
                    id = 10,
                    pnp = "*1.7.4971*",
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "7200",
                    variable = "WakeupInterval"
                },
                {
                    id = 11,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1",
                    variable = "Configured"
                },
                {
                    id = 12,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "0x7,",
                    variable = "AlarmType"
                },
                {
                    id = 13,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "0,1",
                    variable = "AlarmVersion"
                },
                {
                    id = 14,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = ",0x7,",
                    variable = "SubscribedAlarms"
                },
                {
                    id = 16,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1,3,25",
                    variable = "SensorMlType"
                },
                {
                    id = 17,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1,6,0,12,7,12,7",
                    variable = "PlusInfo"
                },
                {
                    id = 18,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "271,2065,12631",
                    variable = "FirmwareInfo"
                },
                {
                    id = 19,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "X",
                    variable = "AgiInfo"
                },
                {
                    id = 20,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1,65535,7200,1",
                    variable = "WakeupCapabilities"
                },
                {
                    id = 21,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "7200",
                    variable = "ConfiguredWakeupInterval"
                },
                {
                    id = 22,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "",
                    variable = "ConfiguredAssoc"
                },
                {
                    id = 23,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1,15,6,30,24,0,40,200,42,3600,60,10,62,900,64,0,66,0,",
                    variable = "VariablesGet"
                },
                {
                    id = 24,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1-Motion detection sensitivity (8-255),1d,,6-Motion detection-alarm cancellation delay,2d,,24-Tamper operating modes (0-2),1d,,40-Illuminance report-threshold,2d,,42-Illuminance report-interval,2d,,60-Temperature report-threshold,2d,,62-Temperature measuring-interval,2d,,64-Temperature report-interval,2d,,66-Temperature offset,2d,",
                    variable = "ConfiguredVariable"
                },
                {
                    id = 25,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1:;2:A;3:;4:A",
                    variable = "ModeSetting"
                },
                {
                    id = 26,
                    service = "urn:micasaverde-com:serviceId:SecuritySensor1",
                    value = "0",
                    variable = "Armed"
                },
                {
                    id = 27,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1628669237",
                    variable = "LastUpdate"
                },
                {
                    id = 28,
                    service = "urn:micasaverde-com:serviceId:HaDevice1",
                    value = "1628669237",
                    variable = "FirstConfigured"
                }
            },
            subcategory_num = "3",
            time_created = "1628669207"
        },
        {
            altid = "m1",
            category_num = 17,
            device_file = "D_TemperatureSensor1.xml",
            device_json = "",
            device_type = "urn:schemas-micasaverde-com:device:TemperatureSensor:1",
            disabled = 0,
            embedded = "1",
            id = 60,
            id_parent = 59,
            impl_file = "",
            ip = "",
            mac = "",
            manufacturer = "",
            model = "",
            name = "",
            no_child_sync = "1",
            room = "0",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1",
                    variable = "SensorMlType"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1",
                    variable = "SensorMlScale"
                },
                {
                    id = 3,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "",
                    variable = "VariablesGet"
                }
            },
            subcategory_num = 0,
            time_created = "1628669225"
        },
        {
            altid = "m3",
            category_num = 18,
            device_file = "D_LightSensor1.xml",
            device_json = "",
            device_type = "urn:schemas-micasaverde-com:device:LightSensor:1",
            disabled = 0,
            embedded = "1",
            id = 61,
            id_parent = 59,
            impl_file = "",
            ip = "",
            mac = "",
            manufacturer = "",
            model = "",
            name = "",
            no_child_sync = "1",
            room = "0",
            states = {
                {
                    id = 0,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "3",
                    variable = "SensorMlType"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "2",
                    variable = "SensorMlScale"
                },
                {
                    id = 3,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "",
                    variable = "VariablesGet"
                }
            },
            subcategory_num = 0,
            time_created = "1628669225"
        },
        {
            altid = "m25",
            category_num = 4,
            device_file = "D_GenericSensor1.xml",
            device_json = "",
            device_type = "urn:schemas-micasaverde-com:device:GenericSensor:1",
            disabled = 0,
            embedded = "1",
            id = 62,
            id_parent = 59,
            impl_file = "",
            ip = "",
            mac = "",
            manufacturer = "",
            model = "",
            name = "",
            no_child_sync = "1",
            room = "0",
            states = {
                {
                    id = 1,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "25",
                    variable = "SensorMlType"
                },
                {
                    id = 2,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "1",
                    variable = "SensorMlScale"
                },
                {
                    id = 3,
                    service = "urn:micasaverde-com:serviceId:ZWaveDevice1",
                    value = "",
                    variable = "VariablesGet"
                }
            },
            subcategory_num = 0,
            time_created = "1628669225"
        }
    },
    downtime = {
        "1627472319-8",
        "1627472480-11",
        "1627472559-9",
        "1627472613-14",
        "1627473006-8",
        "1627473179-113",
        "1627473206-7",
        "1627473408-82",
        "1627548770-75362",
        "1627548781-11",
        "1627549033-7",
        "1627549044-11",
        "1627549985-16",
        "1627550044-14",
        "1627562427-12308",
        "1627562442-10",
        "1627562455-8",
        "1627562477-7",
        "1627564437-15",
        "1627564520-8",
        "1627564583-13",
        "1627891002-317244",
        "1627891013-11",
        "1627891121-8",
        "1627891133-12",
        "1627915946-24488",
        "1627915958-12",
        "1627920206-8",
        "1627920218-12",
        "1627920345-7",
        "1627977079-55929",
        "1627977095-11",
        "1627977108-8",
        "1627977121-8",
        "1627977430-14",
        "1628147217-167092",
        "1628147227-10",
        "1628147374-7",
        "1628147385-11",
        "1628147512-12",
        "1628147564-12",
        "1628147644-15",
        "1628148294-590",
        "1628148305-11",
        "1628149492-7",
        "1628149504-12",
        "1628149583-14",
        "1628149660-12",
        "1628668677-518937",
        "1628668779-7",
        "1628668791-12",
        "1628668908-7",
        "1628669039-11",
        "1628669103-14",
        "1628669173-15"
    },
    energy_dev_log = "",
    firmware_version = "1",
    gmt_offset = "-11",
    ip_requests = {
        {
            date = 1586449811,
            ip = "192.168.0.100",
            mac = "C4:98:80:F2:BD:D6"
        },
        {
            date = 1586449914,
            ip = "192.168.0.102",
            mac = "58:E6:BA:3C:33:63"
        },
        {
            date = 1587557214,
            ip = "192.168.0.98",
            mac = "02:42:56:F4:5C:CC"
        },
        {
            date = 1587540643,
            ip = "192.168.0.107",
            mac = "00:0D:6F:00:27:22"
        },
        {
            date = 1628668751,
            ip = "192.168.0.97",
            mac = "E4:E4:AB:A3:4C:D4"
        },
        {
            date = 1628148378,
            ip = "192.168.0.87",
            mac = "96:CD:E9:8D:ED:E6"
        },
        {
            date = 1627979953,
            ip = "192.168.0.84",
            mac = "F0:18:98:78:D3:6D"
        },
        {
            date = 1627891300,
            ip = "192.168.0.89",
            mac = "D4:6A:6A:BA:DA:1F"
        },
        {
            date = 1627979930,
            ip = "192.168.0.88",
            mac = "6E:0C:33:68:3F:F6"
        },
        {
            date = 1627301658,
            ip = "192.168.0.82",
            mac = "3C:59:1E:BB:6E:DB"
        },
        {
            date = 1627549752,
            ip = "192.168.0.86",
            mac = "02:42:60:69:1C:1C"
        },
        {
            date = 1627979249,
            ip = "192.168.0.83",
            mac = "A6:58:41:2B:7B:E5"
        },
        {
            date = 1627916090,
            ip = "192.168.0.101",
            mac = "F8:34:41:1F:DC:EF"
        }
    },
    ir = 0,
    latitude = "49.9808",
    local_udn = "uuid:4d494342-5342-5645-0000-00000348c363",
    longitude = "36.2527",
    mode_change_delay = 30,
    mode_change_mode = "",
    mode_change_time = "",
    model = "Sercomm G550",
    modelID = 37,
    net_pnp = 1,
    room_sync = "1,main,2,roomA,3,roomB,4,roomC",
    rooms = {
        {
            id = 1,
            name = "main",
            section = 1
        },
        {
            id = 2,
            name = "roomA",
            section = 1
        },
        {
            id = 3,
            name = "roomB",
            section = 1
        },
        {
            id = 4,
            name = "roomC",
            section = 1
        }
    },
    scene_sync = "",
    scenes = {},
    sections = {
        {
            id = 1,
            name = "My Home"
        }
    },
    setup_wizard_finished = "1",
    shouldHelpOverlayBeHidden = true,
    skin = "mios",
    sync_kit = "0000-00-00 00:00:00",
    timezone = "0",
    users = {
        {
            IsGuest = 0,
            Level = 1,
            Name = "volodymyr.nikolaev",
            id = 2677212
        }
    },
    weatherSettings = {
        tempFormat = "F",
        weatherCity = "Kharkiv, Kharkiv",
        weatherCountry = "Ukraine"
    },
    zwave_backup = {
        homeid = "fd217d71",
        nif = {
            {
                id = 1,
                nif = "0x28 0xb0 0xbd 0x76 0xa0 0xc7"
            },
            {
                id = 2,
                nif = "0x28 0xb0 0xbd 0x76 0xf0 0x93"
            },
            {
                id = 3,
                nif = "0x28 0xb0 0xbd 0x76 0xc8 0xa"
            }
        },
        nodeid = 1
    },
    zwave_heal = "1628668781"
}
