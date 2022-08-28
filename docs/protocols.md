# Protocols and Standards

@tableofcontents

This library implements the application layer as defined in the following protocols:
- [ECSS-E-ST-70-41C](https://ecss.nl/standard/ecss-e-st-70-41c-space-engineering-telemetry-and-telecommand-packet-utilization-15-april-2016/)
  (Packet Utilisation Standard)
- [CCSDS 133.0-B-2](https://public.ccsds.org/Pubs/133x0b2e1.pdf) (Space Packet)

<table>
<tr>
<td colspan="4" align="center">Space Packet</td>
</tr>
<tr>
<td colspan="1" align="center">Space Packet Primary Header</td>
<td colspan="3" align="center">Packet Data Field</td>
</tr>
<tr>
<td colspan="1" align="center">Space Packet Primary Header</td>
<td colspan="1" align="center">ECSS Secondary Header</td>
<td colspan="1" align="center">ECSS Data Field</td>
<td colspan="1" align="center">ECSS Error Control</td>
</tr>
</table>

These protocols are supposed to be encapsulated in protocols representing the higher layers. For example, an RF message
may be placed in a CCSDS 132.0-B-3 frame, while a message transmitted via wire may be placed inside a CAN bus frame.
The user of this library must specify how a message is received and transmitted (see @ref usage-mcu).

The headers defined by the different protocols define different attributes of the message, such as:
- Its length
- Its source
- Its destination
- A counter to identify lost messages
- The time it was generated
- Whether a response is expected on success
- The number of the ECSS service of the message

The **error control** field embeds an **Error-Detecting Code** to investigate if the message was transmitted and
received correctly, and was not corrupted by faults in the communication channel. It is managed by @ref
Message::finalize.

