SELECT 
icinga_instances.instance_id
,icinga_instances.instance_name
,icinga_comments.object_id
,obj1.name1 AS host_name
,obj1.name2 AS service_description
,icinga_comments.*
FROM `icinga_comments`
LEFT JOIN icinga_objects as obj1 ON icinga_comments.object_id=obj1.object_id
LEFT JOIN icinga_instances ON icinga_comments.instance_id=icinga_instances.instance_id
WHERE obj1.objecttype_id='2'
ORDER BY entry_time DESC, entry_time_usec DESC, comment_id DESC

