#
# Generated by erpcgen 1.10.0 on Sun May 28 20:10:52 2023.
#
# AUTOGENERATED - DO NOT EDIT
#

import erpc
from . import common, interface

# Client for BBCarService
class BBCarServiceClient(interface.IBBCarService):
    def __init__(self, manager):
        super(BBCarServiceClient, self).__init__()
        self._clientManager = manager

    def stop(self, cars):
        # Build remote function invocation message.
        request = self._clientManager.create_request()
        codec = request.codec
        codec.start_write_message(erpc.codec.MessageInfo(
                type=erpc.codec.MessageType.kInvocationMessage,
                service=self.SERVICE_ID,
                request=self.STOP_ID,
                sequence=request.sequence))
        if cars is None:
            raise ValueError("cars is None")
        codec.write_uint8(cars)

        # Send request and process reply.
        self._clientManager.perform_request(request)

    def goStraight(self, cars, speed):
        # Build remote function invocation message.
        request = self._clientManager.create_request()
        codec = request.codec
        codec.start_write_message(erpc.codec.MessageInfo(
                type=erpc.codec.MessageType.kInvocationMessage,
                service=self.SERVICE_ID,
                request=self.GOSTRAIGHT_ID,
                sequence=request.sequence))
        if cars is None:
            raise ValueError("cars is None")
        codec.write_uint8(cars)
        if speed is None:
            raise ValueError("speed is None")
        codec.write_int32(speed)

        # Send request and process reply.
        self._clientManager.perform_request(request)

    def turn(self, cars, speed, factor):
        # Build remote function invocation message.
        request = self._clientManager.create_request()
        codec = request.codec
        codec.start_write_message(erpc.codec.MessageInfo(
                type=erpc.codec.MessageType.kInvocationMessage,
                service=self.SERVICE_ID,
                request=self.TURN_ID,
                sequence=request.sequence))
        if cars is None:
            raise ValueError("cars is None")
        codec.write_uint8(cars)
        if speed is None:
            raise ValueError("speed is None")
        codec.write_int32(speed)
        if factor is None:
            raise ValueError("factor is None")
        codec.write_double(factor)

        # Send request and process reply.
        self._clientManager.perform_request(request)

    def ctl_mode(self):
        # Build remote function invocation message.
        request = self._clientManager.create_request()
        codec = request.codec
        codec.start_write_message(erpc.codec.MessageInfo(
                type=erpc.codec.MessageType.kInvocationMessage,
                service=self.SERVICE_ID,
                request=self.CTL_MODE_ID,
                sequence=request.sequence))

        # Send request and process reply.
        self._clientManager.perform_request(request)

    def RemoteControlAction(self, mode, value, factor):
        # Build remote function invocation message.
        request = self._clientManager.create_request()
        codec = request.codec
        codec.start_write_message(erpc.codec.MessageInfo(
                type=erpc.codec.MessageType.kInvocationMessage,
                service=self.SERVICE_ID,
                request=self.REMOTECONTROLACTION_ID,
                sequence=request.sequence))
        if mode is None:
            raise ValueError("mode is None")
        codec.write_uint8(mode)
        if value is None:
            raise ValueError("value is None")
        codec.write_int32(value)
        if factor is None:
            raise ValueError("factor is None")
        codec.write_double(factor)

        # Send request and process reply.
        self._clientManager.perform_request(request)

    def RemoteTuneSpeed(self, max, min):
        # Build remote function invocation message.
        request = self._clientManager.create_request()
        codec = request.codec
        codec.start_write_message(erpc.codec.MessageInfo(
                type=erpc.codec.MessageType.kInvocationMessage,
                service=self.SERVICE_ID,
                request=self.REMOTETUNESPEED_ID,
                sequence=request.sequence))
        if max is None:
            raise ValueError("max is None")
        codec.write_double(max)
        if min is None:
            raise ValueError("min is None")
        codec.write_double(min)

        # Send request and process reply.
        self._clientManager.perform_request(request)

    def RemoteShowSpeed(self):
        # Build remote function invocation message.
        request = self._clientManager.create_request()
        codec = request.codec
        codec.start_write_message(erpc.codec.MessageInfo(
                type=erpc.codec.MessageType.kInvocationMessage,
                service=self.SERVICE_ID,
                request=self.REMOTESHOWSPEED_ID,
                sequence=request.sequence))

        # Send request and process reply.
        self._clientManager.perform_request(request)

    def RemoteShowPattern(self):
        # Build remote function invocation message.
        request = self._clientManager.create_request()
        codec = request.codec
        codec.start_write_message(erpc.codec.MessageInfo(
                type=erpc.codec.MessageType.kInvocationMessage,
                service=self.SERVICE_ID,
                request=self.REMOTESHOWPATTERN_ID,
                sequence=request.sequence))

        # Send request and process reply.
        self._clientManager.perform_request(request)


