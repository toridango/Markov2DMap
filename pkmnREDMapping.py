# -*- coding: UTF-8 -*-

import math
import numpy as np
import json
import cv2
import sys


def saveMapping(mapping, filename):
    with open(filename, 'w') as outfile:
        # json.dump(mapping, outfile, indent=4, sort_keys=True)
        json.dump(mapping, outfile, indent=4)

def loadMapping(filename):
    with open(filename, 'r', encoding='utf-8') as json_data:
        d = json.load(json_data)
        return d

# Print iterations progress
def printProgressBar (iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█'):
    """
    Call in a loop to create terminal progress bar
    @params:
        iteration   - Required  : current iteration (Int)
        total       - Required  : total iterations (Int)
        prefix      - Optional  : prefix string (Str)
        suffix      - Optional  : suffix string (Str)
        decimals    - Optional  : positive number of decimals in percent complete (Int)
        length      - Optional  : character length of bar (Int)
        fill        - Optional  : bar fill character (Str)
    """
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print('\r%s |%s| %s%% %s' % (prefix, bar, percent, suffix), end = '\r')
    # Print New Line on Complete
    if iteration == total: 
        print()



def getMap(img, tileW = 16, tileH = 16):
    
    imgShape = img.shape
    if (imgShape[0]%tileW != 0 or imgShape[1]%tileH != 0):
        raise ValueError("img size not multiple of tiles")

    # cv2 has height first 
    matrixWidth = (int)(imgShape[1] / tileW)
    matrixHeight = (int)(imgShape[0] / tileH)

    print("matrixWidth", matrixWidth, "matrixHeight", matrixHeight)

    starth = 0
    startw = 0
    endh = imgShape[0]
    endw = imgShape[1]

    # dict of label-to-img
    # int: img(tileW x tileH)
    tileDict = []
    # dict of int-to-label
    #itolDict = {}
    # dict of label-to-int
    #ltoiDict = {}
    # map of int labels
    tileMap = np.empty([int((endh-starth)/tileH), int((endw-startw)/tileW)], dtype = int)
    
    
    next_tile = 0
    for i in range(starth, endh, tileH):
        for j in range(startw, endw, tileW):
            # cv2 has height first 
            crop = img[i:i+tileH, j:j+tileW]
            iblock = (int)((i-starth)/tileH)
            jblock = (int)((j-startw)/tileW)
            
            if not tileDict:
                tileDict.append(crop[:])
                tileMap[iblock, jblock] = next_tile
                cv2.imwrite('./blocks/{}.png'.format(next_tile),crop)
                next_tile += 1
                print("Unique tiles found:", next_tile, end="\r")
            else:                
                k = 0
                found = False
                while not found and k < len(tileDict):
                    # v = cv2.subtract(tileDict[k], crop)
                    # if images are the same (all pixels black after subtraction)
                    if cv2.countNonZero(cv2.cvtColor(cv2.subtract(tileDict[k], crop), cv2.COLOR_BGR2GRAY)) == 0:
                        tileMap[iblock, jblock] = k
                        found = True
                    k += 1
                if not found:
                    tileDict.append(crop[:])
                    tileMap[iblock, jblock] = next_tile
                    cv2.imwrite('./blocks/{}.png'.format(next_tile),crop)
                    next_tile += 1
                    print("Unique tiles found:", next_tile, end="\r")


    print()


    saveMapping(tileMap.tolist(), "tileMap.json")



def de_tile_map():
    filename = "pkmnREDMap.png"
    img = cv2.imread(filename)
    getMap(img)



def main():
    de_tile_map()
    # data = loadMapping("tileMap.json")
    # saveMapping(data, "tileMap.json")


if __name__ == "__main__":
    main()