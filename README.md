# SMSBackup-Parser
Parses and extracts files from the Carbonite app [SMS Backup &amp; Restore](https://play.google.com/store/apps/details?id=com.riteshsahu.SMSBackupRestore)

The SMS Backup &amp; Restore app has the option to embed images, videos, etc that are in MMS messages into its backup file.  The backup XML file it generates will contain the base64 encoded data and usually be rather large.  Unfortunately the app itself doesn't give the easy ability to extract the encoded files from the XML file.  This app serves to fill that gap.  

For now, the types this application looks for to extract are:  png, jpeg, gif, and 3gpp.  If you know your way around the code, just add more types as you find them.  These are the only types I found in a few different backups so far.

**Note**   
You must remove the following XSL stylesheet line from your XML file, as TinyXML (the parser used in this app) does not handle this.
```xml
<?xml-stylesheet type="text/xsl" href="sms.xsl"?>
```

### Dependencies
CMake >= 2.8.11

### Build
*Linux*
```
mkdir build
cd build
cmake ..
make
```
*Windows*   
1. Run the CMake GUI   
2. Choose the source (location of the `CMakeLists.txt` file)   
3. Choose the binary directories (usually just append `\build`)   
4. Click Configure (and choose your installed C++ compiler)   
5. Click Generate   
6. Navigate to the binary directory and open your generated project files   


### Usage
```
SMSBackupParser -d <destination_directory> XML_FILE
```
