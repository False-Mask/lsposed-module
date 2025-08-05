#!/usr/bin/env python3
import os
import uuid
import struct
from collections import namedtuple

from perfetto.trace_builder.proto_builder import TraceProtoBuilder
from perfetto.protos.perfetto.trace.perfetto_trace_pb2 import TrackEvent, TrackDescriptor, ProcessDescriptor, ThreadDescriptor

# 定义 Python 数据结构（与 C++ 结构体对应）
LogEntry = namedtuple('LogEntry', ['pname', 'timestamp', 'methodName', 'type'])
def  parse_log_entry(file):
    # 1. 解析 pname
    (pname_len,) = struct.unpack('<B', file.read(1))  # 读取4字节小端无符号整数
    pname = file.read(pname_len).decode('utf-8')  # 读取UTF-8字符串

    # 2. 解析 timestamp
    (timestamp,) = struct.unpack('<q', file.read(8))  # 读取8字节小端有符号整数

    # 3. 解析 methodName
    (method_name_len,) = struct.unpack('<B', file.read(1))
    method_name = file.read(method_name_len).decode('utf-8')

    # 4. 解析 type
    (entry_type,) = struct.unpack('<B', file.read(1))  # 读取4字节小端无符号整数

    return LogEntry(pname, timestamp, method_name, entry_type)

def populate_packets(builder: TraceProtoBuilder):
    """
    This function is where you will define and add your TracePackets
    to the trace. The examples in the following sections will provide
    the specific code to insert here.

    Args:
        builder: An instance of TraceProtoBuilder to add packets to.
    """
    # ======== BEGIN YOUR PACKET CREATION CODE HERE ========
    # Example (will be replaced by specific examples later):
    #
    # packet = builder.add_packet()
    # packet.timestamp = 1000
    # packet.track_event.type = TrackEvent.TYPE_SLICE_BEGIN
    # packet.track_event.name = "My Example Event"
    # packet.track_event.track_uuid = 12345
    #
    # packet2 = builder.add_packet()
    # packet2.timestamp = 2000
    # packet2.track_event.type = TrackEvent.TYPE_SLICE_END
    # packet2.track_event.track_uuid = 12345
    #
    # ========  END YOUR PACKET CREATION CODE HERE  ========
    #
    # Remove this 'pass' when you add your code
    # Define a unique ID for this sequence of packets (generate once per trace producer)
    TRUSTED_PACKET_SEQUENCE_ID = 1001  # Choose any unique integer

    # Define a unique UUID for your custom track (generate a 64-bit random number)
    CUSTOM_TRACK_UUID = 12345678  # Example UUID

    # 1. Define the Custom Track
    # This packet describes the track on which your events will be displayed.
    # Emit this once at the beginning of your trace.
    packet = builder.add_packet()
    packet.track_descriptor.uuid = CUSTOM_TRACK_UUID
    packet.track_descriptor.name = "My Custom Data Timeline"

    # 2. Emit events for this custom track
    # Example Event 1: "Task A"
    packet = builder.add_packet()
    packet.timestamp = 1000  # Start time in nanoseconds
    packet.track_event.type = TrackEvent.TYPE_SLICE_BEGIN
    packet.track_event.track_uuid = CUSTOM_TRACK_UUID  # Associates with the track
    packet.track_event.name = "Task A"
    packet.trusted_packet_sequence_id = TRUSTED_PACKET_SEQUENCE_ID

    packet = builder.add_packet()
    packet.timestamp = 1500  # End time in nanoseconds
    packet.track_event.type = TrackEvent.TYPE_SLICE_END
    packet.track_event.track_uuid = CUSTOM_TRACK_UUID
    packet.trusted_packet_sequence_id = TRUSTED_PACKET_SEQUENCE_ID

    # Example Event 2: "Task B" - a separate, non-nested task occurring later
    packet = builder.add_packet()
    packet.timestamp = 1600  # Start time in nanoseconds
    packet.track_event.type = TrackEvent.TYPE_SLICE_BEGIN
    packet.track_event.track_uuid = CUSTOM_TRACK_UUID
    packet.track_event.name = "Task B"
    packet.trusted_packet_sequence_id = TRUSTED_PACKET_SEQUENCE_ID

    packet = builder.add_packet()
    packet.timestamp = 1800  # End time in nanoseconds
    packet.track_event.type = TrackEvent.TYPE_SLICE_END
    packet.track_event.track_uuid = CUSTOM_TRACK_UUID
    packet.trusted_packet_sequence_id = TRUSTED_PACKET_SEQUENCE_ID

    # Example Event 3: An instantaneous event
    packet = builder.add_packet()
    packet.timestamp = 1900  # Timestamp in nanoseconds
    packet.track_event.type = TrackEvent.TYPE_INSTANT
    packet.track_event.track_uuid = CUSTOM_TRACK_UUID
    packet.track_event.name = "Milestone Y"
    packet.trusted_packet_sequence_id = TRUSTED_PACKET_SEQUENCE_ID
    pass

uniqId = 0

def write_to_packets(builder, logEntry):
    packet = builder.add_packet()
    packet.timestamp = logEntry.timestamp
    packet.track_event.type = TrackEvent.TYPE_SLICE_BEGIN if logEntry.type == 0 else TrackEvent.TYPE_SLICE_END
    packet.track_event.name = logEntry.methodName
    # global uniqId
    packet.track_event.track_uuid = customUUid
    packet.trusted_packet_sequence_id = trackSid
    # uniqId += 1
    pass
customUUid = 12345678
trackSid = 1001
def main():
    # read all pkg
    builder = TraceProtoBuilder()
    intputFile = "tmp/.com.zhiliaoapp.musically.go"
    file_size = os.path.getsize(intputFile)
    global customUUid

    packet = builder.add_packet()
    packet.track_descriptor.uuid = customUUid
    packet.track_descriptor.name = "My Custom Data Timeline"
    with open(intputFile, 'rb') as f:
        while f.tell() < file_size:
            logEntry = parse_log_entry(f)
            write_to_packets(builder, logEntry)
            # print(f"Process: {logEntry.pname}, Time: {logEntry.timestamp}, methodName: {logEntry.methodName}, methodType: {logEntry.type}")
    """
    Initializes the TraceProtoBuilder, calls populate_packets to fill it,
    and then writes the resulting trace to a file.
    """
    output_filename = "tmp/com.zhiliaoapp.musically.go.pftrace"
    with open(output_filename, 'wb') as f:
      f.write(builder.serialize())




    # this is an example
    builder = TraceProtoBuilder()
    populate_packets(builder)

    output_filename = "tmp/my_custom_trace.pftrace"
    with open(output_filename, 'wb') as f:
      f.write(builder.serialize())

    print(f"Trace written to {output_filename}")
    print(f"Open with [https://ui.perfetto.dev](https://ui.perfetto.dev).")

if __name__ == "__main__":
    main()
