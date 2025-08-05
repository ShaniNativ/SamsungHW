
System Overview

The system, was developed as a home assignment for Samsung, simulates data transmission and reception to FLASH memory over a
communication channel. The main focus of this project is early detection of system
failures, which occur when multiple and contiguous memory writes happen within a defined
time frame. Upon detecting such a condition, the system rites a log file with the details of
the relevant writing pattern, and halts further writes to FLASH memory to prevent the
system from getting stuck.

There are three main modules that write in this project:
1. Writing Pattern Generator  - Responsible to convert a user configuration file(YALM format) to a binary File.
2. Frame Transmitter - Reads the generated binary file and transmits it frame by frame to the next module.
3. Writing Pattern Detector - Receives frames and check if there a system failure occur., If so, it logs the failure and stops the transmission.

* For more Explanation about the modules and the system including the UML diagram read this doc: linkToPDf *

  How to run the system:
  1. Run the cmake to create a make file then compile
  2. Run the test_system.out and provide the path to the "config.yaml"
