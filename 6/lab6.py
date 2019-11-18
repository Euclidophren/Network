import os, sys, subprocess

def readFile(filename):
    lines = 0
    fileToRead = open(filename, 'r')
    fileToRead.readline()
    for line in fileToRead:
        print(line)
        lines += 1
    fileToRead.close()

    return lines

def getActiveConnections(osName):
    if(osName == "linux" or osName == "linux2"):
        os.system("ss -4 > ipv4.txt")
        os.system("ss -6 > ipv6.txt")
    elif(osName == "win32"):
        os.system("arp -a inet_addr")

    print(readFile("ipv4.txt") + readFile("ipv6.txt"))

def getStats(osName):
    if(osName == "linux" or osName == "linux2"):
        os.system("netstat --statistics --raw | grep -A 8 Ip:")
    elif(osName == "win32"):
        os.system("netsh interface ipv4 show ipstats")
        os.system("netsh interface ipv6 show ipstats")
 
def main():
    getActiveConnections(sys.platform)
    getStats(sys.platform)

if __name__ == "__main__":
    main()