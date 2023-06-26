import sys

def showValidationError():
    print("[Unsupported arguments]")
    print(" - Place send arguments in one of the following formats")
    print("     - \"rgb(r,g,b)\" or \"r,g,b\" where r,g, and b are the actual values")

def getRawRbgValues(inputValue):
    inputValue = inputValue.replace(" ", "")

    if not inputValue:
        print("[No arguments]")
    isValid = True
    rgbString = inputValue

    if not rgbString or rgbString.count(',') != 2:
        isValid = False


    if not isValid:
        showValidationError()
        return

    return rgbString.split(',')

def main(inputValues):
    # validate arguments
    print(f"got: {inputValues}")
    if len(inputValues) < 2:
        print("[No arguments]")
        return

    startIdx = 1
    rgbValues = []
    if(inputValues[startIdx] == "rgb"):
        # [script, rgb, r, g, b] = 5
        if(len(inputValues) != 5):
            showValidationError()
            return
        for i in range(2,5):
            rgbValues.append(inputValues[i])
    else:
        rgbValues = getRawRbgValues(inputValues[startIdx])

    
    resultStr = ""

    for i in range(0,3):
        curValue = float(rgbValues[i])/255
        if i > 0:
            resultStr += ', '
        resultStr += f'{curValue:.4f}f'


    print(resultStr)


    
main(sys.argv)
