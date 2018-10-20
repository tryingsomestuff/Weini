import sys
import os

def prepare(filename):
   with open(filename) as f:
      lines = f.readlines()
      
      num = -1
      numend = -1
      score = None
      fen = ""
      
      fens = []
      
      debug = False
      
      for line in lines:
         if 'Begin' in line:
            num = line.split(' ')[-1]
            if debug:
               print 'Begin :', num
            
         elif 'End' in line:
            numend = line.split(' ')[-1]
            if debug:
               print 'End :', numend
            
            if score is None or num != numend:
               print "ERROR : bad state"
               return
            
            for i in fens:
               print '{"fen":"',i,'",  "result":',score,'}'
               
            fens = []
            score = None
            
         elif 'FEN' in line:
            fen = line.split(':')[-1].strip('\n')
            if debug:
               print 'FEN :', fen
            
            fens.append(fen)
            
         elif 'Result' in line:
            result = line.split('::')[-1]
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
            else:
               print 'ERROR :: unknown result'
               return
            
         else:
            if debug:
               print 'garbage :', line
      


if __name__ == "__main__":
    prepare(sys.argv[1])

