#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import re
import requests

try: 
  import xml.etree.cElementTree as ET 
except ImportError: 
  import xml.etree.ElementTree as ET 
import sys 

DDF_PATH = 'http://www.openmobilealliance.org/wp/OMNA/LwM2M/DDF.xml'

def getFileName(url):
  m = re.search(r'[^/]+$', url)
  return m.group(0) 

def downloadFile(fileName, url):
  r = requests.get(url) # create HTTP response object
  with open(fileName,'wb') as f:
    f.write(r.content)

def downloadDDFXml():
  name = getFileName(DDF_PATH)
  downloadFile(name, DDF_PATH)
  return name

def downloadObjectXml(ddfFileName):
  tree = ET.parse(ddfFileName)
  root = tree.getroot()

  nodefinedObjId = ''
  i = 0
  for country in root.findall('Item'):
    path = country.find('DDF').text
    objId = country.find('ObjectID').text
    if path:
      print 'downloading ' + path
      downloadFile(getFileName(path), path)
    else:
      if i == 0:
        nodefinedObjId += str(objId)
      else:
        nodefinedObjId += ';' + str(objId)
      i += 1
  print 'not defined object id: ' + nodefinedObjId

if __name__ == "__main__":
  ddfname = downloadDDFXml()
  downloadObjectXml(ddfname)
