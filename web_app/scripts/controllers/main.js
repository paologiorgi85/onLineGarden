'use strict';

/**
 * @ngdoc function
 * @name myAppApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the myAppApp
 */
//angular.module('myAppApp')
angular.module("myAppApp", ['firebase'])
  //.controller('MainCtrl', function ($scope) {
  .controller('MainCtrl', function ($scope, $firebase) {
    $scope.awesomeThings = [
      'HTML5 Boilerplate',
      'AngularJS',
      'Karma'
    ];

    var ref = new Firebase ("https://luminous-heat-4517.firebaseio.com");
    
    var sync = $firebase(ref.child("scheduling"));
    var profileObject = sync.$asObject();
    $scope.items = profileObject;
    //$scope.items = [];

    // Add a Item to the list
    $scope.addItem = function () {

        var scheduling = ref.child("scheduling");
        var schedule = scheduling.child($scope.itemName);

        schedule.set({
            name:   $scope.itemName,
            hour:   $scope.itemHour,
            minute: $scope.itemMinute,
            time:   $scope.itemTime 
        });
        // Clear input fields after push
        $scope.itemName = "";
        $scope.itemHour = "";
        $scope.itemMinute = "";
        $scope.itemTime = "";
    };



    // Add Item to Checked List and delete from Unchecked List
    $scope.remove = function (id) {
        //$scope.items.scheduling.$remove(index);
        //item.remove();
        var itemRef = new Firebase ("https://luminous-heat-4517.firebaseio.com" + '/scheduling/' + id);
        itemRef.remove();
    };
});
