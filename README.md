# SMSBackup-Parser
Parses and extracts files from the Carbonite app SMS Backup &amp; Restore

### Dependencies
CMake >= 2.8.11

### Build
Linux
```
mkdir build
cd build
cmake ..
make
```

### Usage
```
SMSBackupParser.exe -d <destination_directory> XML_FILE
```

Note: You must remove the following XSL stylesheet line from your XML file, as TinyXML (the parser used in this app) does not handle XSL.
```
<?xml-stylesheet type="text/xsl" href="sms.xsl"?>
```
