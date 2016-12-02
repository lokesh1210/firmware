#include "catch.hpp"
#include "spark_wiring_process.h"

SCENARIO("Start a subprocess", "[process]")
{
    Process proc = Process::run("/bin/false");
    proc.wait();
    REQUIRE(proc.exited() == true);
    REQUIRE(proc.exitCode() == 1);
}

SCENARIO("Waits for process to finish", "[process]")
{
    Process proc = Process::run("cat");
    REQUIRE(proc.exited() == false);
    proc.in().close();
    proc.wait();
    REQUIRE(proc.exited() == true);
}

SCENARIO("Captures standard output", "[process]")
{
    Process proc = Process::run("echo 42 followed by something long");
    proc.wait();
    int result = proc.out().parseInt();
    REQUIRE(result == 42);
}

SCENARIO("Accepts standard input", "[process]")
{
    Process proc = Process::run("bc");
    proc.in().println("6 * 7");
    proc.in().close();
    proc.wait();
    int result = proc.out().parseInt();
    REQUIRE(result == 42);
}

SCENARIO("Captures standard error", "[process]")
{
    Process proc = Process::run("./script_out_err.sh");
    proc.wait();
    String out = proc.out().readString();
    String err = proc.err().readString();
    REQUIRE(out.equals("This is ok\n") == true);
    REQUIRE(err.equals("This is an error\n") == true);
    REQUIRE(proc.exitCode() == 3);
}

SCENARIO("Parses output", "[process]")
{
    // Simulate output of vcgencmd measure_temp
    Process proc = Process::run("echo temp=43.5'C");
    proc.wait();
    // The output is temp=43.5'C
    proc.out().find("=");
    float cpuTemp = proc.out().parseFloat();
    REQUIRE(((cpuTemp - 43.5) < 0.01) == true);
}

SCENARIO("Returns an error when a command is not found", "[process]")
{
    Process proc = Process::run("iawghoigwaehogwhaiooihgwahiogwe");
    proc.wait();
    REQUIRE(proc.exitCode() == Process::COMMAND_NOT_FOUND);
}

SCENARIO("Kill stops the process", "[process]")
{
    Process proc = Process::run("cat");
    proc.kill(SIGKILL);
    proc.wait();
    REQUIRE(proc.exited() == true);
    REQUIRE(proc.exitCode() == 128+SIGKILL);
}
