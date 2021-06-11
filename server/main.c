#include "dbInterface.h"

int main(int argc, char **argv)
{
    // initializeDatabase();
    dataPack t = getUser("ghitssierul", "tsk7EHqSs4n3JgYvwEo2");
    FILE *f = fopen("test", "a+");
    for (int i = 0; i < t.msgLength; i++)
    {
        fputc(t.data[i], f);
    }
    fclose(f);
    // // char test1[10] = {0};
    // // intToString(1254, test1);

    return 0;
}

// insertUser("ghaassssaaarul", "plmssssssscom", "sasssssssss456", "dsassssssssssa");
// updateProfilePicture("ghitssierul", fopen("poza.jpeg", "rb"));
// changeUserPassword("ghitssierul", "mmmmmm");
// removeUser("ghitierul");
// addFriend("ghitssierul", "ghaaaaarul");
// removeFriend("ghitssierul", "ghaaaaarul");

//loginUser("ghitssierul", "mmmmmm");
// logoutUser("ghitssierul", "OfbKWsu4gRfVMrt1kXjp");
// deleteOldToken();
// printf("%d\n", isGoodPassword("RRRrRRRR"));
// printf("%d\n", isGoodUsername("RRRrRRRR"));
// printf("%d\n", isPhoneNumber("0757330934"));
// printf("%d\n", isPhoneNumber("075733w934"));
// printf("%d\n", isPhoneNumber("757123423"));
// printf("%d\n", verifyAuthToken("ghitssierul", "tsk7EHqSs4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("ghitssierul", "tsk7ssss4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("PPierul", "tsk7ssss4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("Ahitssierul", "tsk7EHqSs4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("", "tsk7EHqSs4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("", "tsk7EHqSs4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("", ""));
// printf("%d\n", verifyAuthToken("ghitssierul", ""));
// printf("%d\n", verifyAuthToken("", "tsk7EHqSs4n3JgYvwEo2"));