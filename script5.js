cursor = db.stories.find( 
	{ 
		$and : [ {"topic.name" : "Comedy"},
				$or: 
					[ 
						{ "media" : "images" }, 
						{ "media" : "news" } 

					] 
				]
	} );
while(cursor.hasNext()){
    printjson(cursor.next());
}