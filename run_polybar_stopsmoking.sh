if ! pgrep stopsmoking-pol > /dev/null
then
    stopsmoking-polybar &
fi

GREEN='\033[0;32m' 
RED='\033[0;31m'
NC='\033[0m' # No Color
counter=$(cat /tmp/stopsmokingcounter)
echo $counter

if [ -z $counter ]
then
    echo "${GREEN}✓ 0${NC}"
else
    if [ $counter = "0" ]
    then
        echo "${GREEN}✓ ${counter}${NC}"
    else
        echo "${RED}⚠ ${counter}${NC}"
    fi
fi