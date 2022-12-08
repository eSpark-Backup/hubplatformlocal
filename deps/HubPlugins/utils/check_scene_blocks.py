#!/usr/bin/env python3

import sys
import os
import os.path as path
import json
import re

whenBlockFields = [ "blockId", "item_filter", "device_filter", "label", "blockType", "blockOptions", "fields", "parent_device_filter" ]
whenBlockMethodNames = [ "isItemState", "compareNumbers", "isDictonaryValueState", "isButtonState", "isUserLockOperation", "hasAtLeastOneDictionaryValue", "compareNumberRange", "isDetectedInHotzone" ]
whenBlockFieldsValueTypes = [ "item", "string", "token", "float", "int", "bool", "dictionaryId", "array", "array.token", "array.string", "array.int", "array.float", "dictionary.hotzone_match" ]
labels = {}

def checkItemFilter(block):
    result = True
    if "names" not in block:
        print("No item filter names: \n" + json.dumps(block, indent = 2))
        result = False
        
    return result

def checkDeviceFilter(block):
    result = True
    if ("category" not in block) and ("not" not in block):
        print("Device filter should contains \"category\" array or \"not\" object: \n" + json.dumps(block, indent = 2))
        result = False
        
    return result

def checkLabel(block):
    result = True
    if "lang_tag" not in block:
        print("No label lang_tag: \n" + json.dumps(block, indent = 2))
        result = False
    elif " " in block["lang_tag"]:
        print("Label lang_tag should not include spaces: " + block["lang_tag"])
        
    if "text" not in block:
        print("No label text: \n" + json.dumps(block, indent = 2))
        result = False
        
    if result:
        if block["lang_tag"] in labels and labels[block["lang_tag"]] != block["text"]:
            print("Label " + block["lang_tag"] + " has several different values: \n\t" + labels[block["lang_tag"]] + "\n\t" + block["text"])
            result = False
        else:
            labels[block["lang_tag"]] = block["text"]
        
    return result

def checkValueType(value, valueType):
    result = True
    if valueType == "token" or valueType == "string":
        if not type(value) is str:
            print("Value should be string: " + value)
            result = False
            
    if valueType == "float":
        if not type(value) is float:
            print("Value should be float: " + value)
            result = False
            
    if valueType == "int":
        if not type(value) is int:
            print("Value should be int: " + value)
            result = False
            
    if valueType == "bool":
        if not type(value) is bool:
            print("Value should be bool: " + value)
            result = False
            
    return result

def checkBlockType(block):
    result = True
    if block != "when":
        print("Wrong block type: " + block)
        result = False
        
    return result

def checkBlockOptions(block):
    result = True
    if not "method" in block:
        print("No method field: \n" + json.dumps(block, indent = 2))

    method = block["method"]
    
    if "name" in method:
        if method["name"] not in whenBlockMethodNames:
            print("Wrong block option name: " + method["name"])
            result = False
    else:
        print("No block option name: \n" + json.dumps(block, indent = 2))
        result = False
        
    if not "args" in method:
        print("No block option args: \n" + json.dumps(block, indent = 2))
        result = False
        
    return result

def checkFieldOptions(options):
    result = True
    
    for option in options:
        if not "value" in option:
            print("No field option value: \n" + json.dumps(option, indent = 2))
            result = False
    
        if "label" in option:
            if not checkLabel(option["label"]):
                result = False
        else:
            print("No field option label: \n" + json.dumps(option, indent = 2))
            result = False

    return result

def checkValueInOptions(value, options):
    for option in options:
        if option["value"] == value:
            return True
        
    return False

def checkBlockFields(block, declaredFields):
    result = True
    if len(block) != len(declaredFields):
        print("Wrong fields count. Expected: \n" + json.dumps(declaredFields, indent = 2) + "\nActual: \n" + json.dumps(block, indent = 2))
        result = False
    
    for field in block:
        if not "name" in field:
            print("No field name: \n" + json.dumps(field, indent = 2))
            result = False
        elif field["name"] not in declaredFields:
            print("Undeclared field name: " + field["name"])
            result = False
            
        if not "type" in field:
            print("No field type: \n" + json.dumps(field, indent = 2))
            result = False
        elif not field["type"] in whenBlockFieldsValueTypes:
            print("Field type has unsupported format: " + field["type"])
            result = False
            
        if "options" in field:
            if not checkFieldOptions(field["options"]):
                result = False
        
        if field["name"] != "item":
            if not "value" in field:
                print("No field value: \n" + json.dumps(field, indent = 2))
                result = False
            elif not checkValueType(field["value"], field["type"]):
                result = False
                
            if "options" in field:
                if not checkValueInOptions(field["value"], field["options"]):
                    print("Field value should be in options: " + field["value"])
                    result = False
        else:
            if "value" in field:
                print("Field value specified for \"item\" field. Delete it: \n" + json.dumps(field, indent = 2))
                result = False
        
    return result

def checkWhenBlockId(blockId):
    result = True
    if blockId in checkWhenBlockId.blockIds:
        print("Block Id is duplicated: " + blockId + "\n" )
        result = False
    else:
        checkWhenBlockId.blockIds.add(blockId)
    return result

def checkWhenBlock(block):
    result = True
    
    if "item_filter" in block:
        if not checkItemFilter(block["item_filter"]):
            result = False
    else:
        print("No item filter: \n" + json.dumps(block, indent = 2))
        result = False

    if "device_filter" in block:
        if not checkDeviceFilter(block["device_filter"]):
            result = False
        
    if "label" in block:
        if not checkLabel(block["label"]):
            result = False
    else:
        print("No block label: \n" + json.dumps(block, indent = 2))
        result = False
        
    if "blockType" in block:
        if not checkBlockType(block["blockType"]):
            result = False
    else:
        print("No block type: \n" + json.dumps(block, indent = 2))
        result = False
         
    if "blockOptions" in block:
        if not checkBlockOptions(block["blockOptions"]):
            result = False
    else:
        print("No block options: \n" + json.dumps(block, indent = 2))
        result = False
         
    if "fields" in block:
        if not checkBlockFields(block["fields"], block["blockOptions"]["method"]["args"]):
            result = False
    else:
        print("No block fields: \n" + json.dumps(block, indent = 2))
        result = False
        
    if "blockId" in block:
        if not checkWhenBlockId(block["blockId"]):
            result = False
    else:
        print("No block blockId: \n" + json.dumps(block, indent = 2))
        result = False
        
    for blockField in block:
        if blockField not in whenBlockFields:
            print("Extra block field: " + blockField)
            result = False
            
    return result

def checkWhen(whenBlocks):
    result = True
    checkWhenBlockId.blockIds = set()
    for block in whenBlocks:
        if not checkWhenBlock(block):
            result = False
            
    return result
    

def checkBlocks(jsonFile):
    result = True
    data = json.load(open(jsonFile))
    
    if not "version" in data:
        print("No version specified")
        result = False
        
    if "when" in data:
        if not checkWhen(data["when"]):
            result = False
    else:
        print("No blocks specified")
        result = False
        
    return result
    

def main():
    print( "Scene blocks format check started:" )
    if len(sys.argv) == 1:
        print( "Define file with blocks:" )
        print( "\check_scene_blocks.py <JSON_FILE>" )
        return -2
    
    result = 0
    for eachArg in sys.argv[1:]:
        if not checkBlocks(eachArg):
            result = -1;

    if result == 0:
        print( "Everything looks good!" )
    else:
        print( "You need to fix mentioned issues!" )

    return result


if __name__ == '__main__':
    sys.exit(main())
