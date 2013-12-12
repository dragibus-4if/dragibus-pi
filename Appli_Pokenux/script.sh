for i in `seq 1 10`; do
  sleep 0.1
  clear
  for j in `seq 1 $i`; do echo -n "."; done
  echo ".......˛,..,˛"
  for j in `seq 1 $i`; do echo -n "."; done
  echo ".....,°_,··,_°,"
  for j in `seq 1 $i`; do echo -n "."; done
  echo ".....˙·.,˘,.·˙"
done

echo "Qui de Sacha et Régis, sont rival, va attraper en premier les 151 pokemons ?"
echo "Sacha a une priorité de $1 et Régis une de $2"

nice -n $1 ./op1.sh &
pid_sacha=$!
nice -n $2 ./op2.sh &
pid_regis=$!

wait $pid_sacha
wait $pid_regis

# echo "C'est Sasha qui a attrapé les 151 pokémons en premier"
