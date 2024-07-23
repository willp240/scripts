### Code to use github api to print merged PRs since a given date
### Intended use is for writing release notes
### Originally by Jamie (presumably)
### Modified for python 3 / Alma 9 and extended to generic snoplus
###     repos by Will Parker <william.parker@physics.ox.ac.uk>

import pycurl
from io import StringIO
import urllib
from io import BytesIO
import base64
import sys
import json
buffer = BytesIO()
# Aproach with search api

if len(sys.argv) != 3:
  print("Please supply a repository name and base. eg:\npython list_prs.py rat 2023-12-08")
  exit(1)
  
date = sys.argv[2]
baserepo = sys.argv[1]

base_url="https://api.github.com/search/issues"
repo = "snoplus/" + baserepo
query = "sort=updated&per_page=100&q=repo:{repo}+type:pr+is:merged+merged:>={date}".format(repo=repo,date=date)
parsed_url="{base}?{query}".format(base=base_url,query=query)
headers = {'Accept' : 'application/vnd.github.mercy-preview+json'}

print ("Testing URL : ", parsed_url)

c = pycurl.Curl()
c.setopt(c.HTTPHEADER, ["%s: %s" % t for t in headers.items()])
c.setopt(c.USERPWD, 'willp240:')
c.setopt(c.URL, parsed_url)
c.setopt(c.VERBOSE, True)
c.setopt(c.WRITEFUNCTION, buffer.write)
c.perform()
c.close()

body = buffer.getvalue()

print (body)

# Body is a
# In Python 2, we can print it without knowing what the encoding is.
print ("")
print ("")
print ("")
print ("")
print ("")
print ("")
print ("==== Here's the answer ===")
response = json.loads(body)
print ("Found {0} PRs that have been merged".format(response["total_count"]))
# Pretty print:
for pr in response["items"]:
  print ('* #{num} : {title}'.format(num=pr['number'], title=pr['title']))
