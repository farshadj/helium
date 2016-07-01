'''
Created on 30 Jun 2016

@author: farshadjavadi
'''
import unittest
import os
import sys, getopt
import helium
import string
import random


class test_system():
    
    DATASTORE_URL = '/tmp/4gb'
    DATASTORE_NAME = 'DATASTORE'
    DATASTORE_FLAGS = 15
        
    def setUp(self):
        os.system("dd if=/dev/zero of={} bs=1k count=$((4*1024*1024))".format(self.DATASTORE_URL));
        print("datastore file created at he://./{}".format(self.DATASTORE_URL))
        pass


    def tearDown(self):
        os.system("rm -rf {}".format(self.DATASTORE_URL));
        pass


    def testName(self):

        print("Openning Handler...")
        h = helium.open("he://./{}".format(self.DATASTORE_URL), self.DATASTORE_NAME, self.DATASTORE_FLAGS)
   
        if (h is None):
            print("Handler failed to open %d".format(commitRet))

            # generating 1000 sample 
        print("Inserting 100 key,value...")
       
        for i in range(0, 100):
            sample = self.data_generator(10)
            print("Inserting key {}_{} and value {}_{}".format(sample,'key',sample,'value'))
            insertRet = helium.insert(h, "{}_{}".format(sample, 'key'), "{}_{}".format(sample, 'value'));
            if (insertRet !=0):
                 print("Insert failed with code {}".format(insertRet))
        
        print("100 values successfully inserted")
        
        print("Commiting...")
        commitRet = helium.commit(h);
        print("commit returns {}".format(commitRet))
        
        print("Closing the handler...")
        closeRet = helium.close(h);
        print("closing with code {}".format(closeRet))

        print("Done. Test Finished")
        pass

    def data_generator(self, size=6, chars=string.ascii_uppercase + string.digits):
        return ''.join(random.choice(chars) for _ in range(size))

if __name__ == "__main__":
    # import sys;sys.argv = ['', 'Test.testName']
    test_system=test_system()
    test_system.setUp()
    test_system.testName()
    test_system.tearDown()
