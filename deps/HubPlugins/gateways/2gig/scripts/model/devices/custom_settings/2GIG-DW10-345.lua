return {
    {
        tag = "dw_detection_source",
        label = "Sensing contact selection",
        description = "Select sinfle contact or mixed contact rule to detect sensor state",
        type = "token",
        enum = {
            magnet_contact = "Use only magnet contact",
            external_contact = "Use only external dry contact",
            mixed_optimistic = "Use both contacts, door or window will remain closed if any contact closed",
            mixed_pessimistic = "Use both contacts, door or window will remain opened if any contact opened"
        },
        default = "magnet_contact",
    }
}
