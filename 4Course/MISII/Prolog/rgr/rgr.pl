:-dynamic(consumedProduct/7).

% consumedProduct(N, Name, Weight, Cost, AnimalSpec, Nickname, CageNumber)
consumedProduct(1, meat, 3, 250, tiger, bobik, 6).
consumedProduct(2, pickles, 1, 60, zebra, oleg, 2).
consumedProduct(3, watermelon, 3, 300, capybara, ignat, 9).
consumedProduct(4, fish, 5, 430, carakal, shlyopa, 6).
consumedProduct(5, grass, 3, 300, cow, muza, 1).

% 1) check if at least one with cost > 300 and cost < 1000
less_and_more_cost :-
    consumedProduct(_, _, _, Cost, _, _, _),
    Cost > 300,
    Cost < 1000.

% 2) check if at least one with cost < 300 or cost > 1000
less_or_more_cost :-
    consumedProduct(_, _, _, Cost, _, _, _),
    (Cost < 300; Cost > 1000).

% 3) check if at least one where number equals cage number
number_equals_cageNumber :-
    consumedProduct(Number, Name, _, _, _, _, CageNumber),
    Number = CageNumber,
    write('Such record exists: Name is '), write(Name), nl,
    !;
    write('Not Found').
number_equals_cageNumber.

% 4) get products list by input weight
get_products_by_weight :-
    write('Enter weight: '), nl, read(Weight),    
    findall(consumedProduct(N, Name, Weight, Cost, AnimalSpec, Nickname, CageNumber), consumedProduct(N, Name, Weight, Cost, AnimalSpec, Nickname, CageNumber), Products),
    (Products = [] ->
        write('No records found.');
        write('Here is the list of products with weight equal to: '), write(Weight), write(':'), nl, nl,
        print_products(Products)
    ).  
    print_products([]).

print_products([consumedProduct(N, Name, Weight, Cost, AnimalSpec, Nickname, CageNumber)|Rest]) :-
    write('Number: '),
    write(N), nl,
    write('Name of the product: '),
    write(Name), nl,
    write('Weight: '),
    write(Weight), nl,
    write('Cost: '),
    write(Cost), nl,
    write('Animal spec: '),
    write(AnimalSpec), nl,
    write('Animal nickname: '),
    write(Nickname), nl,
    write('Animal cage number: '),
    write(CageNumber), nl,
    get0(C),
    print_products(Rest).

% 5) check if there are two records with equal weight and cost
equal_weight_and_cost :-
    consumedProduct(N1, _, Weight1, Cost1, _, _, _),
    consumedProduct(N2, _, Weight2, Cost2, _, _, _),
    N1 \= N2,
    Weight1 = Weight2,
    Cost1 = Cost2,
    write('Such records found, their numbers are '), write(N1), write(' and '), write(N2), nl,
    !;
    write('Such Records Not Found').