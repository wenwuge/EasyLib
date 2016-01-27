#!/usr/bin/env python
import ConfigParser
import os
import urllib2
import io
import time 

def calMd5(afile):  
    m = hashlib.md5()  
    file = io.FileIO(afile,'r')  
    bytes = file.read(1024)  
    while(bytes != b''):  
        m.update(bytes)  
        bytes = file.read(1024)   
        md5value = m.hexdigest()  
    file.close()   
    return  md5value
                                         

class ItemDesc:
    def __init__(self, name, src, dst, action):
        self.name_=name
        self.source_path_=src
        self.dest_path_=dst
        self.action_=action

    def __str__(self):
        return self.name_ + "\n" + "src:" + self.source_path_ \
                + "\n" + "dst:" + self.dest_path_ + "\n" \
                +"action:" + self.action_
     
class DconfManager:
    def __init__(self):
        self.load_items_ = {}

    def __str__(self):
        return str(self.load_items_)

    def parse(self, conf):
#        [lbnamed_config]
#        srcfile_pattern="http://ccdata.api.360safe.com:8360/rsync_qdnsinfo/data/lbnamed_config"
#        dest_file=/home/s/data/getconf/lbnamed_config
#        exec_command="/home/s/getconf/bin/ipdisadm -d /home/s/data/getconf/lbnamed_config"
        parser = ConfigParser.ConfigParser()
        parser.read(conf)
        sections = parser.sections()
        
        for section in sections:
            src = parser.get(section, "src")
            dst = parser.get(section, "dest")
            action = parser.get(section, "action")
            item = ItemDesc(section, src, dst, action)
            self.load_items_[section] = item

    def init_dconf(self):
        for key in self.load_items_:
            item = self.load_items_[key]
            if os.path.exists(item.dest_ptah_)==false:
                __download(self, item.source_path_, item.dest_path_)
            md5 = calMd5(item.dest_path_);
            file = os.open(item.dest_path_+".md5", w)
            file.write(md5)
            file.close()

        
    def __download(self, url, dest):
        f = urllib2.urlopen()
        data = f. read()
        with open(dest, "wb") as code:
            code.write(data)
        md5 = calMd5(dest);
        file = os.open(dest +".md5", w)
        file.write(md5)
        file.close()

    def compare_and_download(self, section_name):
        item = self.load_items_[section_name]
        #get the source file md5
        md5_path = item.source_path_ + ".md5"
        print md5_path
        f_online = urllib2.urlopen(md5_path)
        md5_online = f_online.read()
        
        #compare the local file md5 with online
        f_local = os.open(item.dest_path_ + ".md5")
        md5_local = f_local.readline()
        f_local.close()
        
        #if not same, download the online file and do action for users
        if md5_local != md5_online:
            __download(self, item.source_path_, item.dest_path_)
            os.system(item.action_)

    def start(self):
        while 1:
            for key in self.load_items_:
                self.compare_and_download(key)

            time.sleep(10)

if __name__ == "__main__":
    dconf = DconfManager()
    dconf.parse("./test.ini")
    #url = "http://crule_test.safe.qihoo.net:8360/CloudRule/data/rules/cloudkill2.xml"
    #dst = "/home/libin3-s/cloudkill.xml"
    #download(url, dst)
    dconf.start()

