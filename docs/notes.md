Question 2.1
------------

## Valeurs de SP :

    (reset)   $sp = 0x0
    (reset)   $sp = 0x8000000 (cause: mov sp, #0x8000000)
    (notmain) $sp = 0x7fffffc (cause: push {lr}
    (notmain) $sp = 0x7fffff0 (cause: sub sp, sp, #12)
    (notmain) $sp = 0x7ffffec (cause: push {lr})
    (foo)     $sp = 0x7ffffe0 (cause: sub sp, sp, #12)
    (bar)     $sp = 0x7ffffd0 (cause: sub sp, sp, #16)
    (bar)     $sp = 0x7ffffe0 (cause: add sp, sp, #16)
    (foo)     $sp = 0x7ffffec (cause: add sp, sp, #12)
    (notmain) $sp = 0x7fffff0 (cause: pop {pc})
    (notmain) $sp = 0x7fffffc (cause: add sp, sp, #12)
    (notmain) $sp = 0x8000000 (cause: pop {pc})

__Note :__
Quand on diminue SP, on empile (et inversement).
La pile crôit donc de haut vers le bas.

## Comparaison SP/adresses variables locales :

Entrée de notmain :

    $sp = 0x7fffff4, &code = 0x7fffffc

L'address de code est en-dessous de sp, ceci s'explique par la nécéssité
d'empiler les paramètres et variables locales (pour les sauvegarder) lorsqu'on
effectue un autre appel à l'intérieur de ce scope.

## Commandes ASM :

    MOV X, Y                    ; mettre Y dans X
    MOV X, #42                  ; mettre la valeur 42 dans X
    STR Rd, [sp, #immed_8x4]    ; stocke un mot dans la mémoire (Rd)
    LDR Rd, [sp, #immed_8x4]    ; charge un mot depuis la mémoire (Rd)
    PUSH {LR}                   ; empile LR (link register - adresse de retour)
    POP {PC}                    ; dépile dans PC (program counter)
