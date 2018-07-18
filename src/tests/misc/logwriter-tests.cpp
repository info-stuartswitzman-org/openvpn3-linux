//  OpenVPN 3 Linux client -- Next generation OpenVPN client
//
//  Copyright (C) 2018         OpenVPN, Inc. <sales@openvpn.net>
//  Copyright (C) 2018         David Sommerseth <davids@openvpn.net>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, version 3 of the
//  License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

/**
 * @file   logwriter.cpp
 *
 * @brief  Simple unit test for the LogWriter interfaces.
 */

#include <iostream>
#include <unistd.h>

#include "log/ansicolours.hpp"
#include "log/logwriter.hpp"

/**
 *  Tests the Write(std::string) method of the LogWriter implementation.
 *  This test will also toggle the timestamp off and on and write
 *  a log line without a timestamp.  This will not cause a difference in
 *  loggers who prepends the timestamp regardless of the input log data
 *  (like syslog).
 *
 * @param w  LogWriter instance to run tests on
 *
 */
void run_test_1(LogWriter& w)
{
    w.Write("**** run_test_1()");
    w.Write("Log line 1 with timestamp");
    w.Write("Log line 2 with timestamp");
    sleep(3);
    w.Write("Log line 3 - 3 seconds later");
    w.EnableTimestamp(false);
    w.Write("Log line 4 - without timestamp");
    w.EnableTimestamp(true);
    w.Write("Log line 5 - with timestamp again");
}


/**
 *  Tests the Write(LogGroup, LogCategoty, std::string) method and will
 *  run a two loops ensuring all valid combinations of LogGroup and
 *  LogCategory are tested.
 *
 * @param w LogWriter instance to run tests on
 */
void run_test_2(LogWriter& w)
{
    w.Write("**** run_test_2()");
    for (int group = 1; group < LogGroupCount; group++)
    {
        for (int catg = 1; catg < 9; catg++)
        {
                w.Write((LogGroup) group, (LogCategory) catg,
                        std::string("LogGroup/LogCategory test line: ")
                        + std::to_string(group) + ":"
                        + std::to_string(catg));
        }
    }
}


/**
 *  Tests the Write(LogEvent(...)) method and will
 *  run a two loops ensuring all valid combinations of LogGroup and
 *  LogCategory are tested.
 *
 * @param w LogWriter instance to run tests on
 */
void run_test_3(LogWriter& w)
{
    w.Write("**** run_test_3()");
    for (int group = 1; group < LogGroupCount; group++)
    {
        for (int catg = 1; catg < 9; catg++)
        {
                LogEvent ev((LogGroup) group, (LogCategory) catg,
                            std::string("LogEvent() test line: ")
                            + std::to_string(group) + ":"
                            + std::to_string(catg));
                w.AddMeta("Meta data for test line:"
                          + std::to_string(group) + ":"
                          + std::to_string(catg));
                w.Write(ev);
        }
    }
}


int main(int argc, char **argv)
{
    // Simple text/plain log writer, logging to stdout
    std::cout << "Testing LogWriter" << std::endl
              << "----------------------------------------------------------"
              << std::endl;
    StreamLogWriter lfw(std::cout);
    run_test_1(lfw);
    run_test_2(lfw);
    run_test_3(lfw);
    std::cout << std::endl << std::endl;

    // Similar to the previous text/plain test, but uses the colours variant.
    // Still logging to stdout

    std::cout << "Testing ColourWriter, mode: by category" << std::endl
              << "----------------------------------------------------------"
              << std::endl;

    auto colours = new ANSIColours();
    ColourStreamWriter cfw(std::cout, colours);
    run_test_1(cfw);
    run_test_2(cfw);
    run_test_3(cfw);
    std::cout << std::endl << std::endl;

    std::cout << "Testing ColourWriter, mode: by group" << std::endl
              << "----------------------------------------------------------"
              << std::endl;
    cfw.SetColourMode(ColourStreamWriter::ColourMode::BY_GROUP);
    run_test_2(cfw);
    run_test_3(cfw);

    // Test the syslog implementation of LogWriter.  To validate these
    // log entries, the syslog log files needs to be evaluated.
    std::cout << "Testing SyslogWriter" << std::endl
              << "----------------------------------------------------------"
              << std::endl;
    SyslogWriter slw("logwriter:syslog", LOG_LOCAL1);
    run_test_1(slw);
    run_test_2(slw);
    run_test_3(slw);
    std::cout << "Check the syslog for results" << std::endl << std::endl;

    std::cout << "All tests done" << std::endl << std::endl;
    return 0;
}
