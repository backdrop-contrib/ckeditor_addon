<?php


/**
 * Ajax callback: Processes image uploads from backdropuploadimage plugin.
 */
function filter_upload_image($entityType, $format) {
  global $user;

  $settings = config_get('filter.format.'.$format, 'editor_settings.image_upload');
  $destination = $settings['scheme'] . '://' . $settings['directory'];
  $destination = token_replace($destination);
  $extensions = 'jpg jpeg gif png';
  $validators['file_validate_extensions'] = array();
  $validators['file_validate_extensions'][0] = $extensions;
  $validators['file_validate_name_length'] = array();
  $response = array('uploaded' => TRUE);

  $data = $_FILES['upload'];
  if ($data['error'] || !is_uploaded_file($data['tmp_name'])) {
    return;
  }

  $file = new File();
  $file->uid = $user->uid;
  $file->status = 1;
  $file->filename = trim(backdrop_basename($data['name']), '.');
  $file->uri = $data['tmp_name'];
  $file->filemime = file_get_mimetype($file->filename);
  $file->filesize = $data['size'];

  $file->filename = file_munge_filename($file->filename, $extensions);

  // Rename potentially executable files, to help prevent exploits (i.e. will
  // rename filename.php.foo and filename.php to filename.php.foo.txt and
  // filename.php.txt, respectively). Don't rename if 'allow_insecure_uploads'
  // evaluates to TRUE.
  if (!settings_get('allow_insecure_uploads', 0) && preg_match('/\.(php|pl|py|cgi|asp|js)(\.|$)/i', $file->filename) && (substr($file->filename, -4) != '.txt')) {
    $file->filemime = 'text/plain';
    $file->uri .= '.txt';
    $file->filename .= '.txt';
    // The .txt extension may not be in the allowed list of extensions. We have
    // to add it here or else the file upload will fail.
    if (!empty($extensions)) {
      $response['message'] = t('For security reasons, your upload has been renamed to %filename.', array('%filename' => $file->filename));
    }
  }

  // Assert that the destination contains a valid stream.
  $destination_scheme = file_uri_scheme($destination);
  if (!$destination_scheme || !file_stream_wrapper_valid_scheme($destination_scheme)) {
    backdrop_set_message(t('The file could not be uploaded, because the destination %destination is invalid.', array('%destination' => $destination)), 'error');
    return FALSE;
  }

  // A URI may already have a trailing slash or look like "public://".
  if (substr($destination, -1) != '/') {
    $destination .= '/';
  }
  $file->destination = file_destination($destination . $file->filename, FILE_EXISTS_RENAME);

  // Call the validation functions specified by this function's caller.
  $errors = file_validate($file, $validators);

  // Check for errors.
  if (!empty($errors)) {
    watchdog('file', 'The specified file %name could not be uploaded.', array('%name' => $file->filename));
    return FALSE;
  }

  file_prepare_directory($destination, $options = FILE_CREATE_DIRECTORY);
  // Move uploaded files from PHP's upload_tmp_dir to Backdrop's temporary
  // directory. This overcomes open_basedir restrictions for future file
  // operations.
  $file->uri = $file->destination;
  if (!backdrop_move_uploaded_file($data['tmp_name'], $file->uri)) {
    watchdog('file', 'Upload error. Could not move uploaded file %file to destination %destination.', array('%file' => $file->filename, '%destination' => $file->uri));
    return FALSE;
  }

  // Set the permissions on the new file.
  backdrop_chmod($file->uri);

  // If we made it this far it's safe to record this file in the database.
  if ($entityType == 'node') {
    $file->save();
    $node = new Node(array(
      'type' => 'image',
      'title' => $file->filename,
      'uid' => $user->uid,
      'status' => 1,
      'field_image' => array ('und' => array(0 => (array) $file)),
    ));
    $node->save();
    $response['url'] = $node->nid;
    $response['message'] = "New node {$node->nid}";
  }
  else {
    $file->status = 0; // explicit TEMPORARY (required for backdropimage)
    $file->save();
    $url = file_create_url($file->uri);
    $url = parse_url($url);
    $url = $url['path'];
    $response['url'] = $file->fid.'@'.$url;
    $response['message'] = "New image {$file->fid}";
  }

  backdrop_json_output($response);
  backdrop_exit();
}
