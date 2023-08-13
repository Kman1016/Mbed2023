# You can write copyrights rules here. These rules will be copied into the outputs.

#
# Generated by erpcgen 1.10.0 on Wed Apr 19 17:10:00 2023.
#
# AUTOGENERATED - DO NOT EDIT
#

import erpc
from . import common, interface

# Client for LEDBlinkService
class LEDBlinkServiceClient(interface.ILEDBlinkService):
    def __init__(self, manager):
        super(LEDBlinkServiceClient, self).__init__()
        self._clientManager = manager

    def led_on(self, led):
        # Build remote function invocation message.
        request = self._clientManager.create_request()
        codec = request.codec
        codec.start_write_message(erpc.codec.MessageInfo(
                type=erpc.codec.MessageType.kInvocationMessage,
                service=self.SERVICE_ID,
                request=self.LED_ON_ID,
                sequence=request.sequence))
        if led is None:
            raise ValueError("led is None")
        codec.write_uint8(led)

        # Send request and process reply.
        self._clientManager.perform_request(request)

    def led_off(self, led):
        # Build remote function invocation message.
        request = self._clientManager.create_request()
        codec = request.codec
        codec.start_write_message(erpc.codec.MessageInfo(
                type=erpc.codec.MessageType.kInvocationMessage,
                service=self.SERVICE_ID,
                request=self.LED_OFF_ID,
                sequence=request.sequence))
        if led is None:
            raise ValueError("led is None")
        codec.write_uint8(led)

        # Send request and process reply.
        self._clientManager.perform_request(request)


