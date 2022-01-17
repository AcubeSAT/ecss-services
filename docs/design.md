# Design

@tableofcontents

The main building block of this library are the @ref Services and the @ref Message.

@dot
digraph G {
  rankdir=LR;

  space [shape=box,label="Spacecraft"]
  ground [shape=box,label="Ground Station"]

  space:se -> ground:sw [label="Response (TM)",color="#039be5"]
  ground:nw -> space:ne  [label="Request (TC)",color="#f4511e"]
}
@enddot

Each Service is represented by a class. All Services reside inside the @ref ServicePool. Messages handle Telecommands
and Telemetry in the form of @ref Message "Messages". A Message represents a _Space Packet_, as defined by [CCSDS
133.0-B-2](https://public.ccsds.org/Pubs/133x0b2e1.pdf). Messages are managed and distributed by the @ref MessageParser class.

## Reading and Writing Messages

The Message is the simplest unit of information that is transferred between space and ground. A Message contains its
payload data in a _binary string_ format, combined by some essential header information.

As the message data is in a binary format, special functions have been written to facilitate reading from and writing to
the Message:
```cpp
// Reading data
message.readUint32();
message.readFloat();

// Writing data
message.appendUint32();
message.appendFloat();

// You can also use templated functions, when the type is obvious and unambiguous
message.append<int16_t>(10);
message.append(someVariable);
```

You can visit @ref Message for a complete description of all functions available for data manipulation.

You will typically need access to `.read()` and `.append()` functions only when developing a Service. Any manipulation
of Messages will typically be done only inside the Service classes.

## The Lifetime of a Message
@dot
digraph H {
    rankdir=LR

    Message [label="TM Message",URL="@ref Message",shape=box,style=filled,color="#039be5"]
    D [label="storeMessage()",fontname="monospace"]

    subgraph cluster_0 {
        style=filled
        color=lightgrey
        node [style=filled,color=white]

        A [label="@ref MessageParser",fontname="monospace"]
        B [label="Service::function()",fontname="monospace"]

        A -> B

        label = "Telemetry (TM) processing";
    }

    subgraph cluster_1 {
        style=filled
        color=lightgrey
        node [style=filled,color=white]

        C [label="Service::function()",fontname="monospace"]
        TCMessage [label="TC Message",shape=box,style=filled,color="#f4511e"]

        C -> TCMessage

        label = "Telecommand (TC) generation";
    }

    Message -> A
    B -> C
    TCMessage -> D
}
@enddot

ecss-services can receive and transmit ECSS messages from and to a ground segment or other subsystems. The codebase is
responsible for the parsing, execution and responses to those messages. Additionally, it provides the tools to tailor
the actions of each service to your subsystem.

### Message reception

When any Telemetry is received, it is first sent in raw format to the @ref MessageParser class. The MessageParser is
responsible for reading the headers of the data, converting it to a @ref Message class, and sending it to the relevant
service.

Inside every Service, each TM message type is tied to a single function that receives the Message as an argument. This
function is responsible for parsing and starting the execution of the contents of the message. It will also generate the
relevant TC, if needed.

### Message generation

After creating a Message and populating it with data, it can then be sent on its way. The @ref Service::storeMessage
function is responsible for propagating the TC message to the next layer.

The next step can be:
- Storage of the message for later retransmission (ST[15] service)
- Immediate transmission of the message to its recipient (e.g. via an antenna, ST[14] service)
- Transmission of the message to other observers (e.g. logging for debugging purposes)

Sometimes, you may want to generate TC without any external stimulus. For example, when an event occurs (e.g. antenna
deployed), you will want to notify the ecss-services library to generate the corresponding TC and take any other actions
required (e.g. event-action definitions). To generate arbitrary TC, just call the relevant functions on each Service class.
