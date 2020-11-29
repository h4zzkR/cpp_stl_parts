#include <iostream>
#include <deque>
/*
 * Есть два дека. Мы следим за тем,
 * чтобы в первом количество было всегда либо
 * равное количеству эжлементов второго, либо
 * они отличаются по количеству не более чем на единицу.
 * Это гарантирует, что в первом у нас первая половина
 * (или n/2 + 1 в случае нечетного количества элементов),
 * в которую мы всегда можем пихнуть гоблина с привилегиями,
 * а простую челядь отправить в конец второго дека (второй
 * половины).
 */

int main() {
    std::deque <int> goblins1, goblins2;
    char command;
    int num_of_req, goblin, old_goblin;
    int len1 = 0, len2 = 0;
    std::cin >> num_of_req;
    for (int i=0; i<num_of_req; i++){
        std::cin >> command;
        if (command == '+'){
            std::cin >> goblin;
            if (len1 == 0){
                goblins1.push_back(goblin); len1++;
            }
            else {
                goblins2.push_back(goblin); len2++;
            }
        } else if (command == '-') {
            old_goblin = goblins1.front();
            goblins1.pop_front();
            std::cout << old_goblin << std::endl;
            len1--;
        } else if (command == '*') {
            std::cin >> goblin;
            /* Costil detected
             * Гоблины особые = особый подход
             * Следим за тем, чтобы в первом была половина!
             */
            if (len1 - len2 == 1){
                goblins2.push_front(goblin); len2++;
            }
            else { goblins1.push_back(goblin); len1++; }
        }
        // следим за количествами элементов в деках
        if (len1 < len2){
            old_goblin = goblins2.front(); goblins2.pop_front();
            goblins1.push_back(old_goblin);
            len2--; len1++;
        }
//        std::cout << len1 << ' ' << len2 << std::endl;
    }

}
