module.exports = function(grunt) {

  // Project configuration.
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    less: {
      development: {
        options: {
          paths: ["less"]
        },
        files: {
          "less/suwa.css": "less/suwa.less"
        }
      }
    },

    watch: {
      lessfiles: {
        files: ['less/*.less'],
        tasks: ['less']
      },

      livereload: {
        options: {livereload: true },
        files: ['less/*.css']
      }
    }
  });

  // Load the plugin that provides the "uglify" task.
  grunt.loadNpmTasks('grunt-contrib-less');
  grunt.loadNpmTasks('grunt-contrib-watch');

  // Default task(s).
  grunt.registerTask('default', ['less', 'watch']);

};