#include <curses.h>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>

void sig_winch(int signo) {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
  resizeterm(size.ws_row, size.ws_col);
}

int compare(const struct dirent **a, const struct dirent **b) {
  return strcmp(a[0]->d_name, b[0]->d_name);
}

int paging(WINDOW *win, int size, struct dirent **namelist, int n, int col,
           int row, int qq) {
  if (n < 0)
    perror("scandir");
  else {
    wclear(win);
    for (int i = 0 + qq * row; (i < row + qq * row) && i < n; ++i) {
      wattron(win, COLOR_PAIR(1));
      if (i == size) {
        wattron(win, COLOR_PAIR(2));
      }
      wprintw(win, "%s", namelist[i]->d_name);
      for (int j = strlen(namelist[i]->d_name); j < col; ++j) {
        wprintw(win, " ");
      }
    }
  }
  wattron(win, COLOR_PAIR(1));
  wrefresh(win);
  return 0;
}

int main(int argc, char **argv) {
  WINDOW *left_wnd, *sub_left_wnd;
  WINDOW *right_wnd, *sub_right_wnd;

  initscr();
  signal(SIGWINCH, sig_winch);
  curs_set(FALSE);
  start_color();
  refresh();

  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  init_pair(2, COLOR_WHITE, COLOR_CYAN);

  left_wnd = newwin(size.ws_row, size.ws_col / 2, 0, 0);
  wbkgd(left_wnd, COLOR_PAIR(1));
  box(left_wnd, '|', '-');
  right_wnd = newwin(size.ws_row, size.ws_col / 2 - 1, 0, size.ws_col / 2 + 1);
  wbkgd(right_wnd, COLOR_PAIR(1));
  box(right_wnd, '|', '-');

  sub_left_wnd = derwin(left_wnd, size.ws_row - 2, size.ws_col / 2 - 2, 1, 1);
  sub_right_wnd = derwin(right_wnd, size.ws_row - 2, size.ws_col / 2 - 3, 1, 1);

  char buff_left[256], buff_right[256];
  if (getcwd(buff_left, sizeof(buff_left)) == NULL) {
    perror("ошибка getcwd");
  }
  if (getcwd(buff_right, sizeof(buff_right)) == NULL) {
    perror("ошибка getcwd");
  }

  struct dirent **left_namelist, **right_namelist;
  int count_left = scandir(buff_left, &left_namelist, NULL, compare);
  int count_right = scandir(buff_right, &right_namelist, NULL, compare);

  int scurrent_element_left = 0, scurrent_element_right = 0, page_left = 0,
      page_right = 0;

  paging(sub_left_wnd, scurrent_element_left, left_namelist, count_left,
         size.ws_col / 2 - 2, size.ws_row - 2, page_left);

  paging(sub_right_wnd, scurrent_element_right, right_namelist, count_right,
         size.ws_col / 2 - 3, size.ws_row - 2, page_right);

  wrefresh(left_wnd);
  wrefresh(right_wnd);

  keypad(sub_left_wnd, TRUE);
  keypad(sub_right_wnd, TRUE);

  int ch, scurrent_element = scurrent_element_left,
          temp_col = size.ws_col / 2 - 2, temp_count = count_left, key = 1,
          temp_page = 0;
  WINDOW *temp_win = sub_left_wnd;
  struct dirent **temp_namelist = left_namelist;
  char *temp_buff = buff_left;

  while ((ch = wgetch(temp_win)) != 27) {
    int a = temp_count / (size.ws_row - 2);
    a += (a * (size.ws_row - 2) != temp_count);
    switch (ch) {
    case KEY_DOWN:
      scurrent_element += (scurrent_element < temp_count - 1);
      temp_page +=
          ((scurrent_element % (size.ws_row - 2) == 0) && (temp_page < a - 1));
      break;
    case KEY_UP:
      scurrent_element -= (scurrent_element > 0);
      temp_page -= ((scurrent_element % (size.ws_row - 2) == size.ws_row - 3));
      break;
    case 9:
      key ^= 1;
      if (key) {
        scurrent_element_right = scurrent_element;
        page_right = temp_page;
        scurrent_element = scurrent_element_left;
        temp_col = size.ws_col / 2 - 2;
        temp_count = count_left;
        temp_win = sub_left_wnd;
        temp_namelist = left_namelist;
        temp_buff = buff_left;
        temp_page = page_left;
      } else {
        scurrent_element_left = scurrent_element;
        page_left = temp_page;
        scurrent_element = scurrent_element_right;
        temp_col = size.ws_col / 2 - 3;
        temp_count = count_right;
        temp_win = sub_right_wnd;
        temp_namelist = right_namelist;
        temp_buff = buff_right;
        temp_page = page_right;
      }
      break;
    case 10:

      if (strcmp(temp_namelist[scurrent_element]->d_name, "..") == 0) {
        temp_buff[strrchr(temp_buff, '/') - temp_buff] = '\000';
      } else if (strcmp(temp_namelist[scurrent_element]->d_name, ".") != 0) {
        strncat(temp_buff, "/", strlen("/"));
        strncat(temp_buff, temp_namelist[scurrent_element]->d_name,
                strlen(temp_namelist[scurrent_element]->d_name));
        strncat(temp_buff, "\000", strlen("\000"));
      }

      if (!strlen(temp_buff)) {
        temp_buff[0] = '/';
        temp_buff[1] = '\000';
      }
      temp_count = scandir(temp_buff, &temp_namelist, NULL, compare);
      if (temp_count != -1) {
        if (key) {
          scurrent_element_left = 0;
          left_namelist = temp_namelist;
          count_left = temp_count;
          page_left = 0;
        } else {
          scurrent_element_right = 0;
          right_namelist = temp_namelist;
          count_right = temp_count;
          page_right = 0;
        }
        scurrent_element = 0;
        temp_page = 0;
      } else {
        if (key) {
          temp_count = count_left;
          temp_namelist = left_namelist;
        } else {
          temp_count = count_right;
          temp_namelist = right_namelist;
        }
        temp_buff[strrchr(temp_buff, '/') - temp_buff] = '\000';
      }
      break;
    default:
      break;
    }
    paging(temp_win, scurrent_element, temp_namelist, temp_count, temp_col,
           size.ws_row - 2, temp_page);
  }

  for (int i = 0; i < count_left; ++i) {
    free(left_namelist[i]);
  }
  free(left_namelist);

  for (int i = 0; i < count_right; ++i) {
    free(right_namelist[i]);
  }
  free(right_namelist);

  wrefresh(right_wnd);
  wrefresh(left_wnd);
  delwin(right_wnd);
  delwin(left_wnd);
  delwin(sub_right_wnd);
  delwin(sub_left_wnd);
  refresh();
  getch();
  endwin();
  exit(EXIT_SUCCESS);
}
