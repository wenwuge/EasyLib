#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pexpect
import paramiko
import sys

def ssh_cmd(ip,username,passwd, cmd):
    ret = -1
    ssh = pexpect.spawn('ssh %s@%s "%s"' % (username, ip, cmd))
    try:
        i = ssh.expect(['password:', 'continue connecting (yes/no)?'], timeout=5)
        if i == 0 :
            ssh.sendline(passwd)
        elif i == 1:
            ssh.sendline('yes\n')
        
            ssh.expect('password: ')
            ssh.sendline(passwd)
        ssh.sendline(cmd)

        r = ssh.read()
        print r
        ret = 0
    except pexpect.EOF:
        print"EOF"
        ssh.close()
        ret = -1
    except pexpect.TIMEOUT:
        print"TIMEOUT"
        ssh.close()
        ret = -2
    
    return ret 


def ssh2(ip,username,passwd,cmd):
    try:
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(ip,22,username,passwd,timeout=5)
        print "connect ok, cmd is",cmd
        
        #for m in cmd:
        stdin, stdout, stderr = ssh.exec_command(cmd)
        stdin.write("Y")   #简单交互，输入 ‘Y’ 
        out = stdout.readlines()
#屏幕输出
        for o in out:
            print o
        ssh.close()
        print "OK"
    except :
        print'%s\tError\n'%(ip)

if __name__=='__main__':
    host = sys.argv[1]
    username = sys.argv[2]
    passwd = sys.argv[3]
    cmds = sys.argv[4]

    #ssh2(host, username, passwd, cmds)
    ssh_cmd(host, username, passwd, cmds)
