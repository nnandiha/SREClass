//Main Angular application and all of its dependencies
var app = angular.module('ScoringServer', ['ui.router']);

app.config([
  '$stateProvider',
  '$urlRouterProvider',
  function($stateProvider, $urlRouterProvider){
    $stateProvider
      .state('home', {
        url: '/home',
        templateUrl: '/home.html',
        controller: 'MainCtrl',
        resolve: {
          postPromise: ['items', function(items){
            return items.getAllElements();
          }]
        }
      });
      
    $urlRouterProvider.otherwise('/home');
  }
]);

angular.element(document).ready(function(){
  angular.bootstrap(document, ['ScoringServer']);
});

/*
  Factory that manages all of the elements in a single model.
*/
app.factory('items', ['$http', function($http){
  var o = {
    flags: [],
    scores: []
  };
  
  o.getAllElements = function(){
    
    o.getFlags();
    o.getScores();
    return true;
  };
  
  o.getScores = function(){
    return $http.get('/scores/3').success(function(data){
      angular.copy(data, o.scores);
    });
  };
  
  o.getFlags = function(){
    return $http.get('/flags').success(function(data){
      angular.copy(data, o.flags);
    });
  };
  
  return o;
}])

app.controller('MainCtrl', [
  '$scope',
  'items',
  function($scope, items){
    //Make all of the items available to the templating engine
    $scope.flags = items.flags;
    $scope.scores = items.scores;
  }
]);
