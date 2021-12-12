import pycurl
from StringIO import StringIO
import urllib
import cStringIO
import base64
import sys
import json
buffer = StringIO()
# Aproach with search api

headers = { 'Authorization' : 'Basic %s' % base64.b64encode("willp240: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"), 'Accept' : 'application/vnd.github.mercy-preview+json'}
base_url="https://api.github.com/search/issues"
repo = "snoplus/rat"
date = "2021-09-28"
query = "sort=updated&per_page=100&q=repo:{repo}+type:pr+is:merged+merged:>={date}".format(repo=repo,date=date)
parsed_url="{base}?{query}".format(base=base_url,query=query)
print "Testing URL : ", parsed_url
#sys.exit(0)
c = pycurl.Curl()
c.setopt(c.HTTPHEADER, ["%s: %s" % t for t in headers.items()])
c.setopt(c.URL, parsed_url)
#c.setopt(c.POST, 1)
c.setopt(c.VERBOSE, True)
c.setopt(c.WRITEFUNCTION, buffer.write)
c.perform()
c.close()
body = buffer.getvalue()

print body
# Body is a string in some encoding.
# In Python 2, we can print it without knowing what the encoding is.
print ""
print ""
print ""
print ""
print ""
print ""
print "==== Here's the answer ==="
response = json.loads(body)
print "Found {0} PRs that have been merged".format(response["total_count"])
# Pretty print:
for pr in response["items"]:
  print '* #{num} : {title}'.format(num=pr['number'], title=pr['title'].encode('utf-8'))