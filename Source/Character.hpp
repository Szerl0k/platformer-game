class Character{

    /*
     *  Z racji, że w C++ funkcje witrualne nie mogą przyjmować szablonów,
     *  a w wirtualnej metodzie Projectile::update() potrzebowałem przekazać m.in.
     *  1. Satyra, 2. Satyra ALBO dowolnego przeciwnika
     *
     *  Aby tak zrobić postanowiłem w tej metodzie uwzględnić takie argumenty
     *  Character& satyr,
     *  Character& characterType
     *
     *  Wiem, jest to BARDZO nieczytelne, i miesza to w kodzie, ale od wyjaśniania tego typu dziwnych
     *  rozwiazań są właśnie komentarze.
     */


public:
    float xVelocity;

    Character();

    float getXVelocity() const;

};