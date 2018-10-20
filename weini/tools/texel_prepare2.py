import sys
import os

debug=False

def prepare(filename):
   with open(filename) as f:
      lines = f.readlines()
      for line in lines:
         t = line.split()
         result = t[-1]
         fen = ' '.join(t[0:-1])
               
              
         if debug:
            print 'Result :', result
         if '1-0' in result:
            score = 1
            if debug:
               print 'score : ', score
         elif '0-1' in result:
            score = -1
            if debug:
               print 'score : ', score
         elif '1/2-1/2' in result:
            score = 0
            if debug:
               print 'score : ', score
         elif '1/2' in result:
            score = 0
            if debug:
               print 'score : ', score       
         else:
            print 'ERROR :: unknown result', result
            return
               
         print '{"fen":"',fen,'",  "result":',score,'}'      
      
if __name__ == "__main__":
    prepare(sys.argv[1])

