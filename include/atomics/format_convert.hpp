/** @file format_convert.hpp
 *  @brief Function prototypes for the file format
 *
 *  This contains the prototypes of different functions used
 *  to extract information from the output log file.
 *
 *
 */
#ifndef _FORMAT_CONVERT_HPP_
#define _FORMAT_CONVERT_HPP_

/** @brief get component name from log.
 *
 *  This function is invoked to get the component name from the output file
 *
 *  @param data The data from which component to be extracted.
 *  @param component the extracted component
 *  @return void
 *
 */
void getcomponent(char* data, char* component);

/** @brief get port name from log
 *
 *  This function is invoked to get the port name from the output log.
 *
 *  @param data The data from which port to be extracted.
 *  @param port the extracted port
 *  @return void
 */

void getport(char* data, char* port);

/** @brief get port value from log
 *
 * This function is invoked to get the value of port from the output log
 *
 * @param data The data from which value to be extracted
 * @param value The value of the port
 * @return true
 *
 *
 */

bool getvalue(char *data, char *value);

/** @brief to format the output file generated
 *
 *  This function extracts specific information and writes tabulated result
 *  in output file
 *
 *  @param file_input The output log file generated from sender/receiver/subnet/topmodel
 *  @param file_output The formatted output file path
 *
 */

void converter(const char *file_input, const char *file_output);

#endif // _FORMAT_CONVERT_HPP_
