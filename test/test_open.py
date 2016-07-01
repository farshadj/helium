'''
Created on 30 Jun 2016

@author: farshadjavadi
'''
import unittest
import os
import sys, getopt
import helium



class Test(unittest.TestCase):
    
    DATASTORE_URL='/tmp/4gb'
    DATASTORE_NAME="DATASTORE"
    DATASTORE_FLAGS=15
    
        
    def setUp(self):
        os.system("dd if=/dev/zero of={} bs=1k count=$((4*1024*1024))".format(self.DATASTORE_URL));
        pass


    def tearDown(self):
        os.system("rm -rf {}".format(self.DATASTORE_URL));
        pass


    def testName(self):
        print("he://./{}".format(self.DATASTORE_URL))
        
        h=helium.open("he://./{}".format(self.DATASTORE_URL),self.DATASTORE_NAME,self.DATASTORE_FLAGS)
    
        self.assertNotEqual(h, None,'Testing open function')
        pass


if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testName']
    unittest.main()