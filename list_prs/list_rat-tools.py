import pycurl
from StringIO import StringIO
import urllib
import cStringIO
import base64
import sys
import json
buffer = StringIO()
# Aproach with search api

#headers = { 'Authorization' : 'Basic %s' % base64.b64encode("willp240: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"), 'Accept' : 'application/vnd.github.mercy-preview+json'}
headers = { 'Authorization' : 'Basic %s' % base64.b64encode("willp240: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"), 'Accept' : 'application/vnd.github.mercy-preview+json'}
base_url="https://api.github.com/search/issues"
repo = "snoplus/rat-tools"
date = "2023-07-24"
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
