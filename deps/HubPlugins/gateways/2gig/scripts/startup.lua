local twogig = assert(require("2gig"), "Failed to load 2gig functionality")

twogig.subscribe("HUB:2gig/scripts/events/events_handling")

loadfile("HUB:2gig/scripts/helpers/pin_codes_restrictions/process_restrictions")()
