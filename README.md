# pydownload
The file path is defined in python file

1、Execute the python script and download the IPSO XML file

    download.py
    
2、Compiling a C file

    gcc outline.c -lexpat -o outline

3、Print out the downloaded IPSO XML file in the Convention format

    ls ~/IPSO/pub/reg/xml/*.xml | xargs -i ./outline {}
