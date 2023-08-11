#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <conio.h>
#include <time.h>

#define HEARTS "\u2661"
#define DIAMONDS "\u2662"
#define SPADES "\u2660"
#define CLUBS "\u2663"
#define HORIZONTAL_LINE "%c", '-'
#define VERTICAL_LINE "%c", '|'
#define LEFT_UPPER_CORNER "\u250C"
#define RIGHT_UPPER_CORNER "\u2510"
#define LEFT_LOWER_CORNER "\u2514"
#define RIGHT_LOWER_CORNER "\u2518"

#define SINGLE_PLAYER_MODE 0x01
#define MULTI_PLAYER_MODE 0x02

#define FACE_UP 0x01
#define REVERSE_UP 0x02

#define STAND (1 << 0)
#define HIT (1 << 1)
#define DOUBLE (1 << 2)
#define SURRENDER (1 << 3)
#define INSURANCE (1 << 4)
#define SPLIT (1 << 5)

char game_mode;

typedef struct
{
    char *user_name;
    char *password;
    int property;
} User_Info;

int user_num;
User_Info *user_list;

typedef struct
{
    int point;
    int decor;
    int state;
} Poker;

typedef struct
{
    int player_index;
    int bet_amount;
    int poker_num;
    Poker *pokers;
} Player;

int players_num;
Player *players;

int poker_cnt[13][4];
int game_rounds = 3, deck = 6;

Poker deal_cards(int state)
{
    // sleep(1);
    // srand((unsigned)time(NULL));
    Poker poker;
    int decor, point;

    while (1)
    {
        point = rand() % 13;
        decor = rand() % 4;
        if (poker_cnt[point][decor] < deck)
        {
            ++poker_cnt[point][decor];
            break;
        }
        srand((unsigned)rand());
    }

    poker.point = point;
    poker.decor = decor;
    poker.state = state;

    return poker;
}

void read_str(char **str)
{
    int size = 1;
    *str = (char *)malloc(sizeof(char));

    char ch = getchar();
    while (ch != '\n')
    {
        (*str)[size - 1] = ch;
        *str = (char *)realloc(*str, ++size);
        ch = getchar();
    }
    (*str)[size - 1] = '\0';
    // printf("%s\n", *str);
}

int check_str(char *str)
{
    int len = strlen(str), i = 0;
    while (i < len && ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z')))
    {
        ++i;
    }
    return i == len;
}

int check_user(char *str)
{
    int i = 0;
    while (i < user_num)
    {
        // printf("user_name: %s, str: %s\n", user_list[i].user_name, str);
        // sleep(1000);
        if (strcmp(user_list[i].user_name, str) == 0)
        {
            break;
        }
        ++i;
    }
    if (i < user_num)
    {
        return i;
    }
    else
    {
        return -1;
    }
}

void sign_up()
{
    // clrscr();
    printf("\nBlackJackist\n");
    printf("----------------------------------------\n");
    printf("请输入用户名: ");

    ++user_num;
    user_list = (User_Info *)realloc(user_list, user_num * sizeof(User_Info));

    while (1)
    {
        char *str = NULL;
        read_str(&str);

        if (!check_str(str))
        {
            // clrscr();
            printf("\nBlackJackist\n");
            printf("----------------------------------------\n");
            printf("用户名不合法 ( 需仅包含数字和字母 )\n请重新输入: ");
            continue;
        }

        user_list[user_num - 1].user_name = str;

        // free(str);
        break;
    }

    // clrscr();
    printf("\nBlackJackist\n");
    printf("----------------------------------------\n");
    printf("请输入密码: ");
    while (1)
    {
        char *str = NULL;
        read_str(&str);

        if (!check_str(str))
        {
            // clrscr();
            printf("\nBlackJackist\n");
            printf("----------------------------------------\n");
            printf("密码不合法 ( 需仅包含数字和字母 )\n请重新输入: ");
            continue;
        }

        user_list[user_num - 1].password = str;

        // free(str);
        break;
    }

    user_list[user_num - 1].property = 10000;

    FILE *fp = NULL;
    fp = fopen("user_data.txt", "w");
    fprintf(fp, "%d\n", user_num);
    for (int i = 0; i < user_num; ++i)
    {
        // printf("user_name: %s\n", user_list[i].user_name);
        fprintf(fp, "%s %s %d\n", user_list[i].user_name, user_list[i].password, user_list[i].property);
    }
    fclose(fp);

    // clrscr();
    printf("\nBlackJackist\n");
    printf("----------------------------------------\n");
    printf("注册成功！\n");
    sleep(1);
}

void display()
{
    printf("\n");
    for (int i = 0; i < players_num; ++i)
    {
        if (!(i > 0 && players[i].player_index == players[i - 1].player_index))
        {
            printf("%s:\n", user_list[players[i].player_index].user_name);
        }
        for (int j = 0; j < players[i].poker_num; ++j)
        {
            printf(LEFT_UPPER_CORNER);
            printf(HORIZONTAL_LINE);
            printf(HORIZONTAL_LINE);
            printf(HORIZONTAL_LINE);
            printf(HORIZONTAL_LINE);
            printf(HORIZONTAL_LINE);
            printf(RIGHT_UPPER_CORNER);
        }
        printf("\n");
        for (int j = 0; j < players[i].poker_num; ++j)
        {
            printf(VERTICAL_LINE);
            printf("     ");
            printf(VERTICAL_LINE);
        }
        printf("\n");
        for (int j = 0; j < players[i].poker_num; ++j)
        {
            printf(VERTICAL_LINE);
            printf(" ");

            if (players[i].pokers[j].state == REVERSE_UP)
            {
                printf("^_^");
            }
            else
            {
                switch (players[i].pokers[j].decor)
                {
                case 0:
                    printf(HEARTS);
                    break;
                case 1:
                    printf(DIAMONDS);
                    break;
                case 2:
                    printf(SPADES);
                    break;
                case 3:
                    printf(CLUBS);
                    break;
                default:
                    break;
                }

                switch (players[i].pokers[j].point)
                {
                case 0:
                    printf(" A");
                    break;
                case 1:
                    printf(" 2");
                    break;
                case 2:
                    printf(" 3");
                    break;
                case 3:
                    printf(" 4");
                    break;
                case 4:
                    printf(" 5");
                    break;
                case 5:
                    printf(" 6");
                    break;
                case 6:
                    printf(" 7");
                    break;
                case 7:
                    printf(" 8");
                    break;
                case 8:
                    printf(" 9");
                    break;
                case 9:
                    printf("10");
                    break;
                case 10:
                    printf(" J");
                    break;
                case 11:
                    printf(" Q");
                    break;
                case 12:
                    printf(" K");
                    break;
                default:
                    break;
                }
            }
            printf(" ");
            printf(VERTICAL_LINE);
        }
        printf("\n");
        for (int j = 0; j < players[i].poker_num; ++j)
        {
            printf(VERTICAL_LINE);
            printf("     ");
            printf(VERTICAL_LINE);
        }
        printf("\n");
        for (int j = 0; j < players[i].poker_num; ++j)
        {
            printf(LEFT_LOWER_CORNER);
            printf(HORIZONTAL_LINE);
            printf(HORIZONTAL_LINE);
            printf(HORIZONTAL_LINE);
            printf(HORIZONTAL_LINE);
            printf(HORIZONTAL_LINE);
            printf(RIGHT_LOWER_CORNER);
        }
        printf("\n");
    }
}

int calc_points(int n, Poker *poker)
{
    int sum = 0, cnt = 0;
    for (int i = 0; i < n; ++i)
    {
        if (poker[i].point > 9)
        {
            sum += 10;
        }
        else
        {
            sum += (poker[i].point + 1);
        }
        if (poker[i].point == 0)
        {
            ++cnt;
        }
    }
    while (cnt > 0)
    {
        if (sum + 10 <= 21)
        {
            sum += 10;
        }
        --cnt;
    }
    return sum;
}

int main()
{
    // clrscr();
    srand((unsigned)time(NULL));
    printf("\nBlackJackist\n");
    printf("----------------------------------------\n");
    printf("欢迎来到 BlackJackist !\n");
    if (access("user_data.txt", F_OK) != -1)
    {
        FILE *fp = NULL;
        fp = fopen("user_data.txt", "r");
        fscanf(fp, "%d", &user_num);
        user_list = (User_Info *)calloc(user_num, sizeof(User_Info));
        for (int i = 0; i < user_num; ++i)
        {
            char user_name[255];
            char password[255];

            fscanf(fp, " %s %s %d", user_name, password, &user_list[i].property);

            user_list[i].user_name = strdup(user_name);
            user_list[i].password = strdup(password);

            // printf("user_name: %s, ", user_list[i].user_name);
            // printf("password: %s, ", user_list[i].password);
            // printf("property: %d\n", user_list[i].property);
        }

        // for (int i = 0; i < user_num; ++i)
        // {
        //     printf("user_name: %s, ", user_list[i].user_name);
        //     printf("password: %s, ", user_list[i].password);
        //     printf("property: %d\n", user_list[i].property);
        // }

        fclose(fp);
    }
    else
    {
        FILE *fp = NULL;
        fp = fopen("user_data.txt", "w");
        user_num = 4;
        user_list = (User_Info *)calloc(user_num, sizeof(User_Info));
        user_list[0].user_name = "#Dealer";
        user_list[0].password = "#Dealer";
        user_list[0].property = 1000000000;
        user_list[1].user_name = "#Computer1";
        user_list[1].password = "#Computer1";
        user_list[1].property = 100000;
        user_list[2].user_name = "#Computer2";
        user_list[2].password = "#Computer2";
        user_list[2].property = 100000;
        user_list[3].user_name = "#Computer3";
        user_list[3].password = "#Computer3";
        user_list[3].property = 100000;
        fprintf(fp, "%d\n", user_num);
        for (int i = 0; i < user_num; ++i)
        {
            fprintf(fp, "%s %s %d\n", user_list[i].user_name, user_list[i].password, user_list[i].property);
        }
        fclose(fp);
    }
    sleep(1);
    printf("游戏模式选择 ( 1-单人游戏模式  2-多人游戏模式 )\n请输入: ");

    // clrscr();
    // printf("\nBlackJackist\n");
    // printf("----------------------------------------\n");

    while (1)
    {
        char *str = NULL;
        read_str(&str);
        if (strcmp(str, "1") == 0)
        {
            game_mode = SINGLE_PLAYER_MODE;
            free(str);
            break;
        }
        if (strcmp(str, "2") == 0)
        {
            game_mode = MULTI_PLAYER_MODE;
            free(str);
            break;
        }
        free(str);

        // clrscr();
        printf("\nBlackJackist\n");
        printf("----------------------------------------\n");
        printf("游戏模式选择非法 ( 1-单人游戏模式  2-多人游戏模式 )\n请重新输入: ");
    }

    if (game_mode == SINGLE_PLAYER_MODE)
    {
        int user_index;
    flag1:
        // clrscr();
        printf("\nBlackJackist\n");
        printf("----------------------------------------\n");
        printf("请输入用户名: ");
        while (1)
        {
            char *str = NULL;
            read_str(&str);

            if (!check_str(str))
            {
                // clrscr();
                printf("\nBlackJackist\n");
                printf("----------------------------------------\n");
                printf("用户名不合法 ( 需仅包含数字和字母 )\n请重新输入: ");
                continue;
            }

            user_index = check_user(str);
            if (user_index == -1)
            {
            flag2:
                // clrscr();
                printf("\nBlackJackist\n");
                printf("----------------------------------------\n");
                printf("用户不存在，是否注册 (Y or N)\n请输入: ");
                str = NULL;
                read_str(&str);
                printf("%s\n", str);
                if (strcmp(str, "Y") != 0 && strcmp(str, "N") != 0)
                {
                    goto flag2;
                }
                if (strcmp(str, "Y") == 0)
                {
                    sign_up();
                }
                goto flag1;
            }
            free(str);
            break;
        }
        // clrscr();
        printf("\nBlackJackist\n");
        printf("----------------------------------------\n");
        printf("请输入密码: ");
        while (1)
        {
            char *str = NULL;
            read_str(&str);

            if (!check_str(str))
            {
                // clrscr();
                printf("\nBlackJackist\n");
                printf("----------------------------------------\n");
                printf("密码不合法 ( 需仅包含数字和字母 )\n请重新输入: ");
                continue;
            }

            if (strcmp(user_list[user_index].password, str) != 0)
            {
                // clrscr();
                printf("\nBlackJackist\n");
                printf("----------------------------------------\n");
                printf("密码错误\n请重新输入: ");
                continue;
            }

            break;
        }
        // clrscr();
        printf("\nBlackJackist\n");
        printf("----------------------------------------\n");
        printf("登录成功，加载中~\n");
        sleep(1);

        players_num = 2;
        players = (Player *)calloc(players_num, sizeof(Player));
        players[0].player_index = 0;
        players[1].player_index = user_index;
    }

    printf("\nBlackJackist\n");
    printf("----------------------------------------\n");
    printf("添加电脑玩家数量 ( 0 - 3 )\n请输入: ");
    while (1)
    {
        int num;
        scanf("%d", &num);

        if (num < 0 || num > 3)
        {
            // clrscr();
            printf("\nBlackJackist\n");
            printf("----------------------------------------\n");
            printf("电脑玩家数量非法 ( 0 - 3 )\n请重新输入: ");
            continue;
        }

        if (num > 0)
        {
            players = (Player *)realloc(players, (players_num + num) * sizeof(Player));
            for (int i = 0; i < num; ++i)
            {
                players[players_num + i].player_index = i + 1;
            }
            players_num += num;
        }

        break;
    }

    printf("\nBlackJackist\n");
    printf("----------------------------------------\n");
    printf("设定游戏轮次\n请输入: ");
    while (1)
    {
        scanf("%d", &game_rounds);

        if (game_rounds <= 0)
        {
            // clrscr();
            printf("\nBlackJackist\n");
            printf("----------------------------------------\n");
            printf("游戏轮次非法\n请重新输入: ");
            continue;
        }

        break;
    }

    printf("\nBlackJackist\n");
    printf("----------------------------------------\n");
    printf("设定扑克牌副数\n请输入: ");
    while (1)
    {
        scanf("%d", &deck);

        if (deck <= 0)
        {
            // clrscr();
            printf("\nBlackJackist\n");
            printf("----------------------------------------\n");
            printf("扑克牌副数非法\n请重新输入: ");
            continue;
        }

        break;
    }

    printf("\nBlackJackist\n");
    printf("----------------------------------------\n");
    printf("游戏开始\n ");
    sleep(1);

    for (int i = 0; i < game_rounds; ++i)
    {
        for (int j = 1; j < players_num; ++j)
        {
            if (players[j].player_index > 3)
            {
                printf("\nBlackJackist\n");
                printf("----------------------------------------\n");
                printf("ROUND %d\n", i + 1);
                printf("\n%s 当前所剩余额为 %d\n请输入您的下注金额: ", user_list[players[j].player_index].user_name, user_list[players[j].player_index].property);
                while (1)
                {
                    scanf("%d", &players[j].bet_amount);
                    if (players[j].bet_amount % 100 != 0)
                    {
                        // clrscr();
                        printf("\nBlackJackist\n");
                        printf("----------------------------------------\n");
                        printf("ROUND %d\n", i + 1);
                        printf("\n下注金额应为100倍数\n请重新输入: ");
                        continue;
                    }
                    if (players[j].bet_amount > user_list[players[j].player_index].property)
                    {
                        // clrscr();
                        printf("\nBlackJackist\n");
                        printf("----------------------------------------\n");
                        printf("ROUND %d\n", i + 1);
                        printf("\n您的余额不足\n请重新输入: ");
                        continue;
                    }

                    break;
                }
            }
            else
            {
                printf("\nBlackJackist\n");
                printf("----------------------------------------\n");
                printf("ROUND %d\n", i + 1);
                printf("\n%s 当前所剩余额为 %d\n其下注金额为: ", user_list[players[j].player_index].user_name, user_list[players[j].player_index].property);
                sleep(1);

                // srand((unsigned)time(NULL));
                players[j].bet_amount = (rand() % 2 + 1) * 100;
                printf("%d", players[j].bet_amount);
                sleep(1);
            }

            user_list[players[j].player_index].property -= players[j].bet_amount;
        }

        printf("\nBlackJackist\n");
        printf("----------------------------------------\n");
        printf("ROUND %d\n", i + 1);
        printf("\n请稍等, 发牌中~\n ");

        memset(poker_cnt, 0, sizeof(poker_cnt));
        players[0].poker_num = 2;
        players[0].pokers = (Poker *)calloc(2, sizeof(Poker));
        players[0].pokers[0] = deal_cards(FACE_UP);
        players[0].pokers[1] = deal_cards(REVERSE_UP);

        for (int j = 1; j < players_num; ++j)
        {
            players[j].poker_num = 2;
            players[j].pokers = (Poker *)calloc(2, sizeof(Poker));
            players[j].pokers[0] = deal_cards(FACE_UP);
            players[j].pokers[1] = deal_cards(FACE_UP);
        }
        printf("\nBlackJackist\n");
        printf("----------------------------------------\n");
        printf("ROUND %d\n", i + 1);
        display();
        sleep(1);

        if (players[0].pokers[0].point > 8 && players[0].pokers[0].point < 13 && players[0].pokers[1].point == 0)
        {
            players[0].pokers[1].state = FACE_UP;
            printf("\nBlackJackist\n");
            printf("----------------------------------------\n");
            printf("ROUND %d\n", i + 1);
            display();
            printf("\n庄家获得 BlackJack, 获得 BlackJack 的玩家取回赌注, 其余玩家失去赌注, 即将进入下一回合~\n");
            for (int j = 1; j < players_num; ++j)
            {
                if ((players[j].pokers[0].point > 8 && players[j].pokers[0].point < 13 && players[j].pokers[1].point == 0) || (players[j].pokers[1].point > 8 && players[j].pokers[1].point < 13 && players[j].pokers[0].point == 0))
                {
                    user_list[players[j].player_index].property += players[j].bet_amount;
                }
            }
            sleep(1);
            continue;
        }

        if (players[0].pokers[0].point == 0)
        {
            int insurance_cnt[players_num];
            memset(insurance_cnt, 0, sizeof(insurance_cnt));
            for (int j = 1; j < players_num; ++j)
            {
                if (players[j].player_index > 3)
                {
                    printf("\nBlackJackist\n");
                    printf("----------------------------------------\n");
                    printf("ROUND %d\n", i + 1);
                    display();
                    printf("\n%s 当前所剩余额为 %d\n", user_list[players[j].player_index].user_name, user_list[players[j].player_index].property);
                    printf("您可进行的操作有: 0-停牌 4-保险\n请输入: ");
                    int allow_option = STAND + INSURANCE, option;
                    while (1)
                    {
                        scanf("%d", &option);
                        if (option > 5 || option < 0 || ((1 << option) & allow_option) == 0)
                        {
                            printf("\nBlackJackist\n");
                            printf("----------------------------------------\n");
                            printf("ROUND %d\n", i + 1);
                            display();
                            printf("\n%s 当前所剩余额为 %d\n", user_list[players[j].player_index].user_name, user_list[players[j].player_index].property);
                            printf("您可进行的操作有: 0-停牌 4-保险\n无效操作, 请重新输入: ");
                            continue;
                        }

                        if ((1 << option) == INSURANCE)
                        {
                            ++insurance_cnt[0];
                            insurance_cnt[insurance_cnt[0]] = j;
                            user_list[players[j].player_index].property -= players[j].bet_amount / 2;
                        }

                        break;
                    }
                }
                else
                {
                    printf("\nBlackJackist\n");
                    printf("----------------------------------------\n");
                    printf("ROUND %d\n", i + 1);
                    display();
                    printf("\n%s 当前所剩余额为 %d\n", user_list[players[j].player_index].user_name, user_list[players[j].player_index].property);
                    printf("其可进行的操作有: 0-停牌 4-保险\n其选择的操作为: ");
                    sleep(1);

                    // srand((unsigned)time(NULL));
                    int option = rand() % 2;
                    switch (option)
                    {
                    case 1:
                        printf("4");
                        ++insurance_cnt[0];
                        insurance_cnt[insurance_cnt[0]] = j;
                        user_list[players[j].player_index].property -= players[j].bet_amount / 2;
                        break;

                    case 0:
                        printf("0");
                        break;
                    default:
                        break;
                    }
                    sleep(1);
                }
            }

            if (players[0].pokers[1].point > 8 && players[0].pokers[1].point < 13)
            {
                players[0].pokers[1].state = FACE_UP;
                printf("\nBlackJackist\n");
                printf("----------------------------------------\n");
                printf("ROUND %d\n", i + 1);
                display();
                printf("\n庄家拥有 BlackJack,  获得 BlackJack 的玩家取回赌注, 购买保险玩家获得保险金, 其余玩家失去赌注, 即将进入下一回合~\n");
                for (int j = 1; j <= insurance_cnt[0]; ++j)
                {
                    user_list[players[insurance_cnt[j]].player_index].property += players[insurance_cnt[j]].bet_amount * 2;
                }
                for (int j = 1; j < players_num; ++j)
                {
                    user_list[players[j].player_index].property += players[j].bet_amount;
                }
                sleep(1);
                continue;
            }

            printf("\nBlackJackist\n");
            printf("----------------------------------------\n");
            printf("ROUND %d\n", i + 1);
            display();
            printf("\n继续当前回合~\n");
            sleep(1);
        }

        for (int j = 1; j < players_num; ++j)
        {
            if (players[j].player_index > 3)
            {
                printf("\nBlackJackist\n");
                printf("----------------------------------------\n");
                printf("ROUND %d\n", i + 1);
                display();
                printf("\n%s 当前所剩余额为 %d\n", user_list[players[j].player_index].user_name, user_list[players[j].player_index].property);
                printf("您可进行的操作有: ");
                int allow_option, option;
                if (calc_points(players[j].poker_num, players[j].pokers) <= 21)
                {
                    allow_option = STAND + HIT + DOUBLE + SURRENDER;
                    printf("0-停牌 1-加牌 2-双倍下注 3-投降 ");

                    if (players[j].poker_num == 2 && players[j].pokers[0].point == players[j].pokers[1].point)
                    {
                        allow_option += SPLIT;
                        printf("5-分牌 ");
                    }
                }
                else
                {
                    allow_option = STAND;
                    for (int k = 0; k < players[j].poker_num; ++k)
                    {
                        players[j].pokers[k].state = REVERSE_UP;
                    }
                    printf("0-停牌 ");
                }

                printf("\n请输入: ");
                while (1)
                {
                    scanf("%d", &option);
                    if (option > 5 || option < 0 || ((1 << option) & allow_option) == 0)
                    {
                        printf("\nBlackJackist\n");
                        printf("----------------------------------------\n");
                        printf("ROUND %d\n", i + 1);
                        display();
                        printf("\n%s 当前所剩余额为 %d\n", user_list[players[j].player_index].user_name, user_list[players[j].player_index].property);
                        printf("您可进行的操作有: ");
                        int allow_option;
                        if (calc_points(players[j].poker_num, players[j].pokers) <= 21)
                        {
                            allow_option = STAND + HIT + DOUBLE + SURRENDER;
                            printf("0-停牌 1-加牌 2-双倍下注 3-投降 ");

                            if (players[j].poker_num == 2 && players[j].pokers[0].point == players[j].pokers[1].point)
                            {
                                allow_option += SPLIT;
                                printf("5-分牌 ");
                            }
                        }
                        else
                        {
                            allow_option = STAND;
                            for (int k = 0; k < players[j].poker_num; ++k)
                            {
                                players[j].pokers[k].state = REVERSE_UP;
                            }
                            printf("0-停牌 ");
                        }

                        printf("\n无效操作, 请重新输入: ");
                        continue;
                    }
                    break;
                }
                if ((1 << option) == SPLIT)
                {
                    user_list[players[j].player_index].property -= players[j].bet_amount;
                    ++players_num;
                    players = (Player *)realloc(players, players_num * sizeof(Player));
                    for (int k = players_num - 1; k > j; --k)
                    {
                        players[k] = players[k - 1];
                    }
                    players[j].pokers[1] = deal_cards(FACE_UP);
                    players[j + 1].pokers[0] = players[j + 1].pokers[1];
                    players[j + 1].pokers[1] = deal_cards(FACE_UP);
                    --j;
                }
                if ((1 << option) == SURRENDER)
                {
                    user_list[players[j].player_index].property += players[j].bet_amount / 2;
                    players[j].bet_amount /= 2;
                    for (int k = 0; k < players[j].poker_num; ++k)
                    {
                        players[j].pokers[k].state = REVERSE_UP;
                    }
                }
                if ((1 << option) == DOUBLE)
                {
                    user_list[players[j].player_index].property -= players[j].bet_amount;
                    players[j].bet_amount *= 2;
                    ++players[j].poker_num;
                    players[j].pokers = (Poker *)realloc(players[j].pokers, players[j].poker_num * sizeof(Poker));
                    players[j].pokers[players[j].poker_num - 1] = deal_cards(FACE_UP);
                    if (calc_points(players[j].poker_num, players[j].pokers) > 21)
                    {
                        --j;
                    }
                }
                if ((1 << option) == HIT)
                {
                    ++players[j].poker_num;
                    players[j].pokers = (Poker *)realloc(players[j].pokers, players[j].poker_num * sizeof(Poker));
                    players[j].pokers[players[j].poker_num - 1] = deal_cards(FACE_UP);
                    --j;
                }
            }
            else
            {
                printf("\nBlackJackist\n");
                printf("----------------------------------------\n");
                printf("ROUND %d\n", i + 1);
                display();
                printf("\n%s 当前所剩余额为 %d\n", user_list[players[j].player_index].user_name, user_list[players[j].player_index].property);
                printf("其可进行的操作有: ");
                int allow_option, option;
                if (calc_points(players[j].poker_num, players[j].pokers) <= 21)
                {
                    allow_option = STAND + HIT + DOUBLE + SURRENDER;
                    printf("0-停牌 1-加牌 2-双倍下注 3-投降 ");

                    if (players[j].poker_num == 2 && players[j].pokers[0].point == players[j].pokers[1].point)
                    {
                        allow_option += SPLIT;
                        printf("5-分牌 ");
                    }
                }
                else
                {
                    allow_option = STAND;
                    for (int k = 0; k < players[j].poker_num; ++k)
                    {
                        players[j].pokers[k].state = REVERSE_UP;
                    }
                    printf("0-停牌 ");
                }

                printf("\n其选择的操作为: ");
                sleep(1);
                while (1)
                {
                    // srand((unsigned)time(NULL));
                    option = rand() % 6;
                    if (((1 << option) & allow_option) == 0)
                    {
                        continue;
                    }
                    if (calc_points(players[j].poker_num, players[j].pokers) > 15 && (((1 << option) == DOUBLE) || ((1 << option) == HIT)))
                    {
                        continue;
                    }
                    break;
                }
                if ((1 << option) == SPLIT)
                {
                    user_list[players[j].player_index].property -= players[j].bet_amount;
                    ++players_num;
                    players = (Player *)realloc(players, players_num * sizeof(Player));
                    for (int k = players_num - 1; k > j; --k)
                    {
                        players[k] = players[k - 1];
                    }
                    players[j].pokers[1] = deal_cards(FACE_UP);
                    players[j + 1].pokers[1] = deal_cards(FACE_UP);
                    --j;
                }
                if ((1 << option) == SURRENDER)
                {
                    user_list[players[j].player_index].property += players[j].bet_amount / 2;
                    players[j].bet_amount /= 2;
                    for (int k = 0; k < players[j].poker_num; ++k)
                    {
                        players[j].pokers[k].state = REVERSE_UP;
                    }
                }
                if ((1 << option) == DOUBLE)
                {
                    user_list[players[j].player_index].property -= players[j].bet_amount;
                    players[j].bet_amount *= 2;
                    ++players[j].poker_num;
                    players[j].pokers = (Poker *)realloc(players[j].pokers, players[j].poker_num * sizeof(Poker));
                    players[j].pokers[players[j].poker_num - 1] = deal_cards(FACE_UP);
                    if (calc_points(players[j].poker_num, players[j].pokers) > 21)
                    {
                        --j;
                    }
                }
                if ((1 << option) == HIT)
                {
                    ++players[j].poker_num;
                    players[j].pokers = (Poker *)realloc(players[j].pokers, players[j].poker_num * sizeof(Poker));
                    players[j].pokers[players[j].poker_num - 1] = deal_cards(FACE_UP);
                    --j;
                }

                printf("%d", option);
                sleep(1);
            }
        }

        players[0].pokers[1].state = FACE_UP;
        printf("\nBlackJackist\n");
        printf("----------------------------------------\n");
        printf("ROUND %d\n", i + 1);
        display();
        printf("\n庄家决策中~\n");
        sleep(1);
        while (1)
        {
            if (calc_points(players[0].poker_num, players[0].pokers) >= 17)
            {
                break;
            }
            // srand((unsigned)time(NULL));
            int option = rand() % 2;
            if ((1 << option) == HIT)
            {
                ++players[0].poker_num;
                players[0].pokers = (Poker *)realloc(players[0].pokers, players[0].poker_num * sizeof(Poker));
                players[0].pokers[players[0].poker_num - 1] = deal_cards(FACE_UP);
                printf("\nBlackJackist\n");
                printf("----------------------------------------\n");
                printf("ROUND %d\n", i + 1);
                display();
                printf("\n庄家决策中~\n");
                sleep(1);
                continue;
            }
            sleep(1);
            break;
        }

        for (int j = 1; j < players_num; ++j)
        {
            if (calc_points(players[j].poker_num, players[j].pokers) == 21 && players[j].poker_num == 2)
            {
                user_list[players[j].player_index].property += players[j].bet_amount * 2.5;
                printf("%s 赢得了赌注 %d\n", user_list[players[j].player_index].user_name, players[j].bet_amount * 3 / 2);
                continue;
            }
            int loser_flag = 1;
            for (int k = 0; k < players[j].poker_num; ++k)
            {
                if (players[j].pokers[k].state == FACE_UP)
                {
                    loser_flag = 0;
                    break;
                }
            }
            if (calc_points(players[0].poker_num, players[0].pokers) > 21)
            {
                if (loser_flag)
                {
                    printf("%s 输掉了赌注 %d\n", user_list[players[j].player_index].user_name, players[j].bet_amount);
                }
                else
                {
                    user_list[players[j].player_index].property += players[j].bet_amount * 2;
                    printf("%s 赢得了赌注 %d\n", user_list[players[j].player_index].user_name, players[j].bet_amount);
                }
                continue;
            }
            if (loser_flag)
            {
                printf("%s 输掉了赌注 %d\n", user_list[players[j].player_index].user_name, players[j].bet_amount);
                continue;
            }
            if (calc_points(players[0].poker_num, players[0].pokers) < calc_points(players[j].poker_num, players[j].pokers))
            {
                user_list[players[j].player_index].property += players[j].bet_amount * 2;
                printf("%s 赢得了赌注 %d\n", user_list[players[j].player_index].user_name, players[j].bet_amount);
            }
            else if (calc_points(players[0].poker_num, players[0].pokers) > calc_points(players[j].poker_num, players[j].pokers))
            {
                printf("%s 输掉了赌注 %d\n", user_list[players[j].player_index].user_name, players[j].bet_amount);
            }
            else
            {
                user_list[players[j].player_index].property += players[j].bet_amount;
                printf("%s 拿回了赌注 %d\n", user_list[players[j].player_index].user_name, players[j].bet_amount);
            }
        }
    }

    sleep(1);
    printf("\nBlackJackist\n");
    printf("----------------------------------------\n");
    printf("游戏结束, 各玩家剩余金额分别为:\n");
    for (int i = 1; i < players_num; ++i)
    {
        printf("%s 当前所剩余额为 %d\n", user_list[players[i].player_index].user_name, user_list[players[i].player_index].property);
    }
    printf("感谢您的游玩, 再见~");

    return 0;
}
