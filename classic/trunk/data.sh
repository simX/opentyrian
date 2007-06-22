for f in data/*; do
g=`expr "$f" : '\(.*\)' | tr '[:upper:]' '[:lower:]'`
mv "$f" "$g"
done
