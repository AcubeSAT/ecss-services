#ifndef ECSS_SERVICES_MESSAGEPARSER_HPP
#define ECSS_SERVICES_MESSAGEPARSER_HPP

#include <Services/EventActionService.hpp>
#include "Message.hpp"

/**
 * A generic class responsible for the execution and the parsing of the incoming telemetry and telecommand
 * packets
 */

class MessageParser {
public:
	/**
     * This function takes as input TC packets and and calls the proper services' functions that have been
	 * implemented to handle TC packets.
	 *
	 * @param Message Contains the necessary parameters to call the suitable subservice
 	 * @todo Implement the execute() in the upcoming services or generally in the upcoming
 	 * activities
	 */
	static void execute(Message& message);

	/**
	 * Parse a message that contains the CCSDS and ECSS packet headers, as well as the data
	 *
	 * As defined in CCSDS 133.0-B-1
	 *
	 * @param data The data of the message (not null-terminated)
	 * @param length The size of the message
	 * @return A new object that represents the parsed message
	 */
	Message parse(uint8_t* data, uint32_t length);

	/**
	 * @todo: elaborate on this comment
	 * Create a message so that a string can be parsed
	 *
	 * Note: conversion of char* to unsigned char* should flow without any problems according to
	 * this great analysis:
	 * stackoverflow.com/questions/15078638/can-i-turn-unsigned-char-into-char-and-vice-versa
	 */
	Message parseRequestTC(String<ECSS_TC_REQUEST_STRING_SIZE> data);

	/**
	 * @brief Overloaded version
	 * @param data A uint8_t array of the TC packet data
	 * @return Parsed message
	 */
	Message parseRequestTC(uint8_t* data);

	/**
	 * @brief Converts a TC packet of type Message to a String
	 * @details Convert a parsed TC message to a string in order to be used by the services
	 * @param message The Message object to be parsed to a String
	 * @return A String class containing the parsed TC request
	 * @attention The returned String has a fixed size, therefore the message size is considered
	 * fixed and equal to the ECSS_TC_REQUEST_STRING_SIZE definition.
	 */
	String<ECSS_TC_REQUEST_STRING_SIZE> convertTCToStr(Message& message);

private:
	/**
	 * Parse the ECSS Telecommand packet secondary header
	 *
	 * As specified in section 7.4.4.1 of the standard
	 *
	 * @param data The data of the header (not null-terminated)
	 * @param length The size of the header
	 * @param message The Message to modify based on the header
	 */
	void parseTC(const uint8_t* data, uint16_t length, Message& message);

	/**
	 * Parse the ECSS Telemetry packet secondary header
	 *
	 * As specified in section 7.4.3.1 of the standard
	 *
	 * @param data The data of the header (not null-terminated)
	 * @param length The size of the header
	 * @param message The Message to modify based on the header
	 */
	void parseTM(const uint8_t* data, uint16_t length, Message& message);
};

#endif // ECSS_SERVICES_MESSAGEPARSER_HPP
