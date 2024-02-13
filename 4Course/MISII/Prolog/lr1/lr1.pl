:-dynamic

zodiac/4,
zodiac_s/4,
dateOfBirth/3,
fullName/2.

% Aries (March 21 – April 19)
% Taurus (April 20 – May 20)
% Gemini (May 21 – June 20)
% Cancer (June 21 – July 22)
% Leo (July 23 – August 22)
% Virgo (August 23 – September 22)
% Libra (September 23 – October 22)
% Scorpio (October 23 – November 21)
% Sagittarius (November 22 – December 21)
% Capricorn (December 22 – January 19)
% Aquarius (January 20 – February 18)
% Pisces (February 19 – March 20)

zodiac(1, fullName('billy', 'harington'), 'cancer', dateOfBirth('22', 'june', '1987')).
zodiac(2, fullName('van', 'darkholm'), 'taurus', dateOfBirth('19', 'may', '1992')).
zodiac(3, fullName('elon', 'musk'), 'leo', dateOfBirth('11', 'august', '1978')).
zodiac(4, fullName('don', 'juan'), 'leo', dateOfBirth('13', 'august', '1978')).
zodiac(5, fullName('stella', 'jognson'), 'leo', dateOfBirth('14', 'august', '1978')).
zodiac(6, fullName('don', 'juan'), 'pisces', dateOfBirth('12', 'february', '2000')).
zodiac(7, fullName('stella', 'juan'), 'pisces', dateOfBirth('19', 'february', '2002')).
zodiac(8, fullName('seyna', 'juan'), 'pisces', dateOfBirth('12', 'february', '2003')).
zodiac(9, fullName('fedya', 'juan'), 'pisces', dateOfBirth('12', 'february', '1998')).
zodiac(10, fullName('chef', 'juan'), 'pisces', dateOfBirth('12', 'february', '1950')).
zodiac(11, fullName('max', 'juan'), 'pisces', dateOfBirth('12', 'february', '1992')).
zodiac(12, fullName('senya', 'musk'), 'leo', dateOfBirth('11', 'august', '2001')).
zodiac(13, fullName('fedya', 'juan'), 'leo', dateOfBirth('13', 'august', '1999')).
zodiac(14, fullName('stella', 'jognson'), 'leo', dateOfBirth('14', 'august', '1988')).

start:- menu.

menu:-
    repeat, nl,
    write('*******************************'),nl,
    write('* 1. Add record to DB *'),nl,
    write('* 2. Delete record from DB *'),nl,
    write('* 3. Record edit by month of birth'),nl,
    write('* 4. Select all records *'),nl,
    write('* 5. Download DB from file *'),nl,
    write('* 6. Save DB to file *'),nl,
    write('* 7. Relation operations *'),nl,
    write('* 8. Select by month of birth *'),nl,
    write('* 9. Exit *'),nl,
    write('*******************************'), nl ,nl,
    write('Enter menu option with dot at the end!!'),nl,
    read(C),nl,
    proc(C),
    C=9,
    !.

proc(1):-
    write('Finish input with dot!!! :'),nl,
    write('Enter id:'),nl, read(Id),
    write('Enter First Name'),nl, read(FirstName),
    write('Enter Last Name'),nl, read(LastName),
    write('Enter zodiac sign:'),nl, read(ZodiacSign),
    write('Enter Day of birth:'),nl, read(Day),
    write('Enter Month of birth:'),nl, read(Month),
    write('Enter Year of birth:'),nl, read(Year), nl,
    assertz(zodiac(Id, fullName(FirstName, LastName), ZodiacSign, dateOfBirth(Day, Month, Year))),
    write('Enter any symbol'),nl,
    get0(C).

proc(2):-
    write('Enter person id to delete'), nl,
    read(Id),
    retract(zodiac(Id, fullName(_, _), _, dateOfBirth(_, _, _))),
    write('Record:'),tab(2),
    write(Id), tab(2),
    write('was successfully removed from DB'),nl,
    write('Enter any symbol'),nl,
    get0(C), 
    !; 
    write('Requied record:'),tab(2),
    write('not found in DB'),nl,
    write('Enter any symbol'),nl,
    get0(C).

proc(3):-
    write('Write the id of the person: '), nl, read(Id),
    (   retract(zodiac(Id, fullName(_, _), _, dateOfBirth(_, _, _)))
    ->  write('Record exists!'),
        write('Enter First Name'),nl, read(FirstName),
        write('Enter Last Name'),nl, read(LastName),
        write('Enter zodiac sign:'),nl, read(ZodiacSign),
        write('Enter Day of birth:'),nl, read(Day),
        write('Enter Month of birth:'),nl, read(Month),
        write('Enter Year of birth:'),nl, read(Year),
        assertz(zodiac(Id, fullName(FirstName, LastName), ZodiacSign, dateOfBirth(Day, Month, Year))),
        format('Record updated successfully')
    ;   format('Record not found')
    ),
    write('Enter any symbol'), nl,
    get0(C).

proc(4):-
    zodiac(Id, fullName(FirstName, LastName), ZodiacSign, dateOfBirth(Day, Month, Year)),
    nl,
    write('Id: '), write(Id),
    nl,
    write('Full Name: '), write(FirstName), write(' '), write(LastName), tab(2), 
    write('Zodiac Sign: '), write(ZodiacSign), tab(2),
    nl,
    write('Day of birth: '), write(Day),
    nl,
    write('Month of birth: '), write(Month),
    nl,
    write('Year of birth: '), write(Year),
    nl,
    write('Enter any symbol'),
    nl,
    get0(C1),get0(C2), 
    fail; 
    true.

proc(5):-
    see('db_zodiacs.dat'),
    retractall(zodiac(_, fullName(_, _), _, dateOfBirth(_, _, _))),
    db_load,
    seen,
    write('DB data loaded from data successfully'),nl.

db_load:-
    read(Term),
    (Term == end_of_file,!;
    assertz(Term),
    db_load).

proc(6):-
    tell('db_zodiacs.dat'),
    save_db(zodiac(Id, fullName(FirstName, LastName), ZodiacSign, dateOfBirth(Day, Month, Year))),
    told,
    write('DB data uploaded to file: db_zodiacs.dat'),nl.
    
save_db(Term):- 
    Term, 
    write(Term), 
    write('.'),nl,
    fail;
    true.

    proc(7):-
        nl,
            retractall(zodiac_s(_, fullName(_, _), _, dateOfBirth(_, _, _))), %необходимая очисточка, чтобы не копилось после нескольких использований пункта 6
        
            %собираем записи, где магазин - Ашан
        
            write('Formation of the r1 relationship:_zodiacs(of the Auchan store '), nl,
            subset_of_zodiacs(leo,R1), %R1 - список сотрудников филиала 1
            list_in_base(R1), %добавление элементов из R1 в базу данных
            write_list(R1),nl, %вывод списка R1 на экран
         
            %собираем записи, где магазин - Оксана
        
            write('Formation of the r2 relationship:_zodiacs( the Oksana store '), nl,
            subset_of_zodiacs(pisces,R2), %R2 - список сотрудников филиала 2
            list_in_base(R2), %добавление элементов из R2 в базу данных
            write_list(R2),nl, %вывод списка R2 на экран
            
            %Объединение - вывод всех продуктов двух магазинов - Ашана и Оксаны
        
            write('Combined ratio g1_ or g2: '), nl,
            union(Rez1), %Rez1 - список продуктов магазина1 или 2
            write_list(Rez1),nl,
            
            %Пересечение - вывод продуктов, которые есть и в Ашане, и в Оксане, в виде (назван_прод., ашан, цена_ашана, оксана, цена_оксаны)
        
            write('The intersection of the relations g1_ and g2: '), nl,
            intersection(Rez2), %Rez2 - список сотрудников 2-х филиалов 
            write_list(Rez2),nl,
            
            %Разница - выводятся все продукты Ашана, за исключением тех, что есть в Оксане
            write('The difference in the ratio r1-r2: '), nl,
            difference(Rez3), %Rez3-список сотрудников филиала 1 без фил.2
            write_list(Rez3),nl,
            R2 = [], R1 = [], Rez1 = [], Rez2 = [], Rez3 = [],
         
            write('Enter any character'),nl,
            get0(C). %Ожидание ввода символа
        %--------------вспомогалка для 6 пункта----------------------------------------------------
        %формирование подмножества сотрудников R заданного Филиала
        %подмножество R представляется в виде списка термов "сотрудник_ф(...)"
        subset_of_zodiacs(Zodiac,R):-
            bagof(zodiac_s(Id, fullName(FirstName, LastName), Zodiac, dateOfBirth(Day, Month, Year)),
            zodiac(Id, fullName(FirstName, LastName), Zodiac, dateOfBirth(Day, Month, Year)), R).
         
        %правило объединения отношений - r1 или r2
        %объединяются отношения сотрудник_ф(одесса) и сотрудник_ф(киев)
        union_r1_r2(X1, fullName(X2, X3), X4, dateOfBirth(X5,X6,X7)):-
            zodiac_s(X1,fullName(X2, X3), leo, dateOfBirth(X5,X6,X7)),X4=leo;
            zodiac_s(X1,fullName(X2, X3), pisces, dateOfBirth(X5,X6,X7)),X4=pisces.
        
        %формирование списка Rez из фактов "сотрудник_ф1_или_ф2"
        union(Rez):-
            bagof(zodiac_s1_or_s2(X1, fullName(X2, X3), X4, dateOfBirth(X5,X6,X7)),
                 union_r1_r2(X1, fullName(X2, X3), X4, dateOfBirth(X5,X6,X7)), %условие вкл. в список
                 Rez).
        
        %один и тот же продукт в двух магазинах
        intersection_r1_r2(X12, X11, X14, X21, X24):-
            zodiac_s(X11, fullName(X12, X13), leo, dateOfBirth(X15,X16,X17)),X14=leo,
            zodiac_s(X21, fullName(X12, X23), pisces, dateOfBirth(X25,X26,X27)),X24=pisces.
        
        %формирование списка Rez из фактов "сотрудник_ф1_и_ф2"
        intersection(Rez):-
            bagof(product_s1_and_s2(X12, X11, X14, X21, X24),
            intersection_r1_r2(X12, X11, X14, X21, X24), 
            Rez). 
        
        difference_r1_r2(X11, X12, X14,X15,X16,X17):-
            zodiac_s(X11, fullName(X12, X13), leo, dateOfBirth(X15,X16,X17)),X14=leo,
            not(zodiac_s(X21, fullName(X12, X23), pisces, dateOfBirth(X25,X26,X27))),X24=pisces.
        %построение списка Rez из фактов "сотрудник_ф1_и_не_ф2"
        difference(Rez):-
            bagof(product_s1_and_no_s2(X11, X12, X14,X15,X16,X17),
            difference_r1_r2(X11, X12, X14,X15,X16,X17), %условие вкл. в список
            Rez). 
        %добавление термов из списка [H|T] в БД 
        list_in_base([]).
        list_in_base([H|T]):-
            H=zodiac_s(Id, fullName(FirstName, LastName), ZodiacSign, dateOfBirth(Day, Month, Year)),
            assertz(zodiac_s(Id, fullName(FirstName, LastName), ZodiacSign, dateOfBirth(Day, Month, Year))),
            list_in_base(T). %Рекурсивный вызов для след. терма
        %вывод элементов списка [H|T] в каждой строке
        write_list([]).
        write_list([H|T]):-write(H),nl,write_list(T).

proc(8):-
    write('Write the month of the birth: '), nl, read(Month),    
    findall(zodiac(_, fullName(FirstName, LastName), ZodiacSign, dateOfBirth(Day, Month, Year)), zodiac(_, fullName(FirstName, LastName), ZodiacSign, dateOfBirth(Day, Month, Year)), Ids),
    (Ids = [] ->
        write('No records found.');
        write('Here is the list of people born in this month: '), write(Month), write(':'), nl,
        print_zodiacs(Ids)
    ).  
    print_products([]).

print_zodiacs([zodiac(_, fullName(FirstName, LastName), ZodiacSign, dateOfBirth(Day, Month, Year))|Rest]) :-
    write('Full Name: '),
    write(FirstName), write(' '),
    write(LastName), nl,
    write('Zodiac Sign: '),
    write(ZodiacSign), nl,
    write('Date of birth: '),
    write(Day), write(' '),
    write(Month), write(' '),
    write(Year), nl,
    print_products(Rest).

proc(9):-write('Bye-bye, pookie.'),nl.