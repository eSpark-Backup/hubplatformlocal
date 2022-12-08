def check_field_type(field_value, field_type, path):
    assert type(field_value) is field_type, 'field {} must be a {} but is {}'.format(path, field_type.__name__, type(field_value).__name__)

def check_setting_field(setting, field_name, field_type):
    assert field_name in setting, 'field {} must be present'.format(field_name)
    check_field_type(setting[field_name], field_type, field_name)

def check_optional_setting_field(setting, field_name, field_type):
    if field_name in setting:
        check_field_type(setting[field_name], field_type, field_name)

def check_subfield(obj, field_name, field_type, path):
    assert field_name in obj, 'field {}.{} must be present'.format(path, field_name)
    check_field_type(obj[field_name], field_type, '{}.{}'.format(path, field_name))

def check_optional_subfield(obj, field_name, field_type, path):
    if field_name in obj:
        check_field_type(obj[field_name], field_type, '{}.{}'.format(path, field_name))

def validate_action(setting):
    check_setting_field(setting, 'value', int)
    check_setting_field(setting, 'label', dict)
    check_optional_subfield(setting['label'], 'text', str, 'label.text')
    check_optional_subfield(setting['label'], 'lang_tag', str, 'label.lang_tag')

def validate_bool(setting):
    pass

def validate_int(setting):
    check_optional_setting_field(setting, 'valueMin', int)
    check_optional_setting_field(setting, 'valueMax', int)

def validate_rgb(setting):
    check_setting_field(setting, 'components', dict)
    for component_name, value in setting['components'].items():
        check_field_type(value, int, 'components.{}'.format(component_name))

def validate_scalable(setting):
    check_setting_field(setting, 'scale', str)
    check_optional_setting_field(setting, 'valueMin', int)
    check_optional_setting_field(setting, 'valueMax', int)

def validate_token(setting):
    check_setting_field(setting, 'enum', dict)
    for token_name, token_info in setting["enum"].items():
        check_subfield(token_info, 'value', int, 'enum.{}'.format(token_name))
        check_subfield(token_info, 'text', str, 'enum.{}'.format(token_name))

def validate_specific_type(setting):
    validators = {
        'action': validate_action,
        'array.token': validate_token,
        'bool': validate_bool,
        'int': validate_int,
        'rgb': validate_rgb,
        'token': validate_token
    }

    validator = validators.get(setting['type'], validate_scalable)
    validator(setting)

def validate_setting(setting):
    setting.pop("todo", None)

    check_setting_field(setting, 'name', str)
    check_setting_field(setting, 'description', str)
    check_setting_field(setting, 'type', str)
    check_setting_field(setting, 'configuration', dict)
    check_subfield(setting['configuration'], 'number', int, 'configuration.number')
    check_subfield(setting['configuration'], 'size', int, 'configuration.size')
    
    check_optional_setting_field(setting, 'hasSetter', bool)

    validate_specific_type(setting)

def validate_settings(settings):
    for setting in settings:
        validate_setting(setting)
