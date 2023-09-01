local tests = require("tests")
local log = require("logging")
local core = require("core")
local cloud_devices = assert(require "cloud_devices", "Failed to load cloud_devices")

print('Log level is set to: ', log.get_level())

local function create_default_abstract_subscription()
    local create_abstract_request = {
                                       abstractUUID = "1234-abcd",
                                       mappings = {
                                           {
                                               capability = "geofence",
                                               variable = "geofence.1",
                                               valueType = "geofence",
                                               dictionaryType = true,
                                               value = { geofence1 = { modified = 1111 }}
                                           }
                                       }
                                   }

    loadfile("HUB:ezlo_geofencing/scripts/create_abstract_subscription")({
        request = create_abstract_request
    })

    local items = assert(core.get_items(), "Error loading items")
    local item = items[1]

    tests.assert_eq(item.name, "geofence.geofence.1")
    tests.assert_eq(item.value, { geofence1 = { modified = 1111 }})
end

local function delete_abstract_subscription(id, record)
      loadfile("HUB:ezlo_geofencing/scripts/delete_abstract_subscription")({
      request = {abstractUUID = id},
      subscriptionRecord = record
      })
end

local function update_and_delete_abstract_subscription_test()
     create_default_abstract_subscription()
     local update_abstract_request = {
                                        abstractUUID = "1234-abcd",
                                        mappings = {
                                            {
                                                capability = "geofence",
                                                variable = "geofence.1",
                                                valueType = "geofence",
                                                dictionaryType = true,
                                                value = { geofence1 = { modified = 2222 }}
                                            }
                                        }
                                    }

    local abstract_id = update_abstract_request.abstractUUID
    local record = cloud_devices.get_subscription_record(abstract_id)
     loadfile("HUB:ezlo_geofencing/scripts/update_abstract_subscription")({
          request = update_abstract_request,
          subscriptionRecord = record,
          mappingOperations = {update = update_abstract_request.mappings}
     })

     local items = assert(core.get_items(), "Error loading items")
     tests.assert_eq(items[1].value, {geofence1 = {modified = 2222}});
     delete_abstract_subscription(abstract_id, record)
     local items = assert(core.get_items(), "Error loading items")
     tests.assert_eq(items, {})
 end

local function remove_unused_abstract_subscription_test()
      create_default_abstract_subscription()
      local update_abstract_request = {
                                         abstractUUID = "1234-abcd",
                                         mappings = {
                                             {
                                                 capability = "geofence",
                                                 variable = "geofence.1",
                                                 valueType = "geofence",
                                                 dictionaryType = true,
                                                 value = { geofence2 = { modified = 2222 }}
                                             }
                                         }
                                     }

     local abstract_id = update_abstract_request.abstractUUID
     local record = cloud_devices.get_subscription_record(abstract_id)
       loadfile("HUB:ezlo_geofencing/scripts/update_abstract_subscription")({
           request = update_abstract_request,
           subscriptionRecord = record,
           mappingOperations = {update = update_abstract_request.mappings}
       })

     local items = assert(core.get_items(), "Error loading items")
     tests.assert_eq(items[1].value, {geofence2 = {modified = 2222}});

     delete_abstract_subscription(abstract_id, record)
     local items = assert(core.get_items(), "Error loading items")
     tests.assert_eq(items, {})
end

local function update_existing_abstract_subscription_test()
    create_default_abstract_subscription()
    local update_abstract_request = {
                                      abstractUUID = "1234-abcd",
                                      mappings = {
                                          {
                                              capability = "geofence",
                                              variable = "geofence.1",
                                              valueType = "geofence",
                                              dictionaryType = true,
                                              value = { geofence1 = { modified = 2222 }}
                                          }
                                      }
                                  }

    local abstract_id = update_abstract_request.abstractUUID
    local record = cloud_devices.get_subscription_record(abstract_id)
    loadfile("HUB:ezlo_geofencing/scripts/update_abstract_subscription")({
        request = update_abstract_request,
        subscriptionRecord = record,
        mappingOperations = {update = update_abstract_request.mappings}
    })

    local items = assert(core.get_items(), "Error loading items")
    tests.assert_eq(items[1].value, {geofence1 = {modified = 2222}});
    delete_abstract_subscription(abstract_id, record)
    local items = assert(core.get_items(), "Error loading items")
    tests.assert_eq(items, {})
end

local function add_new_abstract_subscription_test()
     create_default_abstract_subscription()
     local update_abstract_request = {
                                        abstractUUID = "1234-abcd",
                                        mappings = {
                                            {
                                                capability = "geofence",
                                                variable = "geofence.1",
                                                valueType = "geofence",
                                                dictionaryType = true,
                                                value = { geofence1 = { modified = 1111 }, geofence2 = { modified = 2222 }}
                                            }
                                        }
                                    }

    abstract_id = update_abstract_request.abstractUUID
    local record = cloud_devices.get_subscription_record(abstract_id)
      loadfile("HUB:ezlo_geofencing/scripts/update_abstract_subscription")({
          request = update_abstract_request,
          subscriptionRecord = record,
          mappingOperations = {update = update_abstract_request.mappings}
      })

      local items = assert(core.get_items(), "Error loading items")
      tests.assert_eq(items[1].value, {geofence2 = {modified = 2222}, geofence1 = {modified = 1111}});
end

update_and_delete_abstract_subscription_test()
remove_unused_abstract_subscription_test()
add_new_abstract_subscription_test()

