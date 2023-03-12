const fs = require("fs");

const { exec } = require("child_process");

let has_changed = false;

fs.watch("test.c", function (event, filename) {
  //   console.log("event is: " + event);
  //   if (filename) {
  //     console.log("filename provided: " + filename);
  //   } else {
  //     console.log("filename not provided");
  //   }

  compileOnChange(event, filename, "gcc test.c -o test_file");

  //   if (event == "change") {
  //     if (filename) {
  //       console.log(filename + " has been altered -- compiling...");
  //       compile("gcc test.c -o test_file");
  //     }
  //   }
});

// gcc `pkg-config gtk+-3.0 --cflags` snake2.c main.c -o out_main_2 `pkg-config gtk+-3.0 --libs`

fs.watch("q_learning.c", function (event, filename) {
  if (!has_changed) {
    //compileOnChange(event, filename, "gcc test.c -o test_file");
    compileOnChange(
      event,
      filename,
      //"gcc `pkg-config gtk+-3.0 --cflags` snake.c main.c -o out_main `pkg-config gtk+-3.0 --libs`"
      "gcc `pkg-config gtk+-3.0 --cflags` q_learning.c snake0.c main.c -o Q_test `pkg-config gtk+-3.0 --libs`"
    );

    has_changed = true;
    setTimeout(() => {
      has_changed = false;
    }, 100);
  }
  // compileOnChange(
  //   event,
  //   filename,
  //   "gcc `pkg-config gtk+-3.0 --cflags` snake.c main.c -o out_main `pkg-config gtk+-3.0 --libs`"
  // );
});

function compileOnChange(event, filename, command) {
  if (event == "change") {
    if (filename) {
      console.log(filename + " has been altered -- compiling...");
      compile(command);
    }
  }
}

function compile(command) {
  exec(command, (error, stdout, stderr) => {
    if (error) {
      console.log(`error: ${error.message}`);
      return;
    }
    if (stderr) {
      console.log(`stderr: ${stderr}`);
      return;
    }
    // console.log(`stdout: ${stdout}`);
    console.log(`OUTPUT: ${stdout}`);
  });

  //   exec("./test_file", (error, stdout, stderr) => {
  //     if (error) {
  //       console.log(`error: ${error.message}`);
  //       return;
  //     }
  //     if (stderr) {
  //       console.log(`stderr: ${stderr}`);
  //       return;
  //     }
  //     // console.log(`stdout: ${stdout}`);
  //     console.log(`OUTPUT: ${stdout}`);
  //     console.log("outpu::: ", parseInt(stdout) + 100);
  //   });
}

// exec("node watched.js", (error, stdout, stderr) => {
//   if (error) {
//     console.log(`error: ${error.message}`);
//     return;
//   }
//   if (stderr) {
//     console.log(`stderr: ${stderr}`);
//     return;
//   }
//   console.log(`stdout: ${stdout}`);
// });
