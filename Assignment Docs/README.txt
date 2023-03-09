Welcome to my CAPS Assignment! 

This archive contains:

CAPSAssignment: The folder containing all the source files for my server.
CAPSClient: The folder containing all the source files for my client/test harness.
CAPSAssignment.exe - the executable for the server.
CAPSClient.exe - the executable for the client/test harness.
CAPSReport.pdf - my throughput evaluation report.
clientserver.bat - a helpful batch file, for running both the client and server in my throughput experiments.
CAPSAssignment.sln - VS project file for the server.
CAPSClient.sln - VS project file for the client/test harness.

You can compile my code in Visual Studio, and there are also executables provided. You can use them to run my server and test harness.
CAPSAssignment.exe is the server application, and takes an IP address as its single command-line argument - i.e. CapsAssignment.exe 127.0.0.1. 
CAPSClient.exe is the client, and takes four command-line arguments - an IP address, the number of poster threads desired by the user, the number of reader threads desired by the user, and the time the user would like those requests to be sent for (in seconds).
My server and test harness can thus be run by running the executables provided like so:
CAPSAssignment.exe IP_ADDRESS
And after the server has been started, in another window:
CAPSClient.exe IP_ADDRESS NUMBER_OF_POSTER_THREADS, NUMBER_OF_READER_THREADS, TIME_IN_SECONDS
A batch file (clientserver.bat) has been included to speed the process up should you wish to reproduce my throughput evaluation experiments. Change the pre-defined command line arguments as necessary.


For proof of execution times surpassing the reference implementation, please refer to the Screenshot Evidence folder, where you will find screenshot proof, with my implementation on the left of each one, for each of the 10 executions required.