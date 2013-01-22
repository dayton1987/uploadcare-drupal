<?php

require_once dirname(__FILE__).'/uploadcare-php/uploadcare/lib/5.2/Uploadcare.php';

/**
 * Implements hook_field_info().
 */
function uploadcare_field_info() {
	return array(
			'uploadcare' => array(
					'label' => t('Uploadcare widget'),
					'description' => t('Uploadcare widget'),
					'default_widget' => 'uploadcare_widget',
					'default_formatter' => 'uploadcare_formatter',
			),
	);
}

function uploadcare_field_settings_form($field, $instance, $has_data) {
	$settings = $field['settings'];
	$form['is_image'] = array(
			'#type' => 'checkbox',
			'#title' => t('Is it an image?'),
			'#required' => FALSE,
			'#default_value' => $field['settings']['is_image'],
			'#description' => t('If the uploaded file is an image you will be able to use file operations'),
	);
	
	$form['crop'] = array(
			'#type' => 'checkbox',
			'#title' => t('Crop'),
			'#default_value' => $field['settings']['crop'],
	);
	$form['crop_width'] = array(
			'#type' => 'textfield',
			'#title' => t('Crop width'),
			'#default_value' => $field['settings']['crop_width'],
	);
	$form['crop_height'] = array(
			'#type' => 'textfield',
			'#title' => t('Crop height'),
			'#default_value' => $field['settings']['crop_height'],
	);		
	$form['crop_center'] = array(
			'#type' => 'checkbox',
			'#title' => t('Center crop?'),
			'#default_value' => $field['settings']['crop_center'],
	);
	$form['crop_fill_color'] = array(
			'#type' => 'textfield',
			'#title' => t('Crop fill color'),
			'#default_value' => $field['settings']['crop_fill_color'],
	);	

	$form['resize'] = array(
			'#type' => 'checkbox',
			'#title' => t('Resize'),
			'#default_value' => $field['settings']['resize'],
	);	
	$form['resize_width'] = array(
			'#type' => 'textfield',
			'#title' => t('Resize width'),
			'#default_value' => $field['settings']['resize_width'],
	);
	$form['resize_height'] = array(
			'#type' => 'textfield',
			'#title' => t('Resize height'),
			'#default_value' => $field['settings']['resize_height'],
	);
	
	$form['scale_crop'] = array(
			'#type' => 'checkbox',
			'#title' => t('Scale crop'),
			'#default_value' => $field['settings']['scale_crop'],
	);
	$form['scale_crop_width'] = array(
			'#type' => 'textfield',
			'#title' => t('Scale crop width'),
			'#default_value' => $field['settings']['scale_crop_width'],
	);
	$form['scale_crop_height'] = array(
			'#type' => 'textfield',
			'#title' => t('Resize crop height'),
			'#default_value' => $field['settings']['scale_crop_height'],
	);
	$form['scale_crop_center'] = array(
			'#type' => 'checkbox',
			'#title' => t('Scale center crop?'),
			'#default_value' => $field['settings']['scale_crop_center'],
	);	

	$form['flip'] = array(
			'#type' => 'checkbox',
			'#title' => t('Flip image?'),
			'#default_value' => $field['settings']['flip'],
	);	
	$form['grayscale'] = array(
			'#type' => 'checkbox',
			'#title' => t('Grayscale image?'),
			'#default_value' => $field['settings']['grayscale'],
	);	
	$form['invert'] = array(
			'#type' => 'checkbox',
			'#title' => t('Invert image?'),
			'#default_value' => $field['settings']['invert'],
	);	
	$form['mirror'] = array(
			'#type' => 'checkbox',
			'#title' => t('Mirror image?'),
			'#default_value' => $field['settings']['mirror'],
	);	
	$form['stretch_off'] = array(
			'#type' => 'checkbox',
			'#title' => t('Stretch off?'),			
			'#default_value' => $field['settings']['stretch_off'],
	);
	
	return $form;
}

function uploadcare_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors)
{
	foreach ($items as $item) {
		if ($item['uploadcare']) {
			$public_key = variable_get('uploadcare_public_key');
			$secret_key = variable_get('uploadcare_secret_key');
			$file_id = $item['uploadcare'];
			$api = new Uploadcare_Api($public_key, $secret_key);
			$file = $api->getFile($file_id);		
			$file->store();
			$filename = $file->data['original_filename'];
			db_query('INSERT INTO {uploadcare} VALUES (:file_id, :filename)', array(':file_id' => $file_id, ':filename' => $filename));
		}
	}
}

/**
 * Implements hook_field_widget_info().
 */
function uploadcare_field_widget_info() {
	return array(
			'uploadcare' => array(
					'label' => t('Uploadcare'),
					'field types' => array('uploadcare'),
			),
	);
}

/**
 * Implements hook_field_formatter_info().
 */
function uploadcare_field_formatter_info() {
	return array(
			'uploadcare_formatter' => array(
					'label' => t('Uploadcare formatter'),
					'field types' => array('uploadcare'),
			),
	);
}

function uploadcare_field_widget_form_postfix($file_id, $settings)
{
	return uploadcare_get_view_html($file_id, $settings);
}

/**
 * Implements hook_field_widget_form().
 */
function uploadcare_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
	switch ($instance['widget']['type']) {
		case 'uploadcare':
			$element['uploadcare'] = array(
			'#prefix' => uplodcare_field_widget_form_prefix(),
			'#suffix' => @uploadcare_field_widget_form_postfix($items[$delta]['uploadcare'], $field['settings']),
			'#type' => 'textfield',
			'#title' => $element['#title'],
			'#description' => $element['#description'],
			'#default_value' => @$items[$delta]['uploadcare'],
			'#required' => $element['#required'],
			'#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
			'#delta' => $delta,
			'#attributes' => array(
				'role' => 'uploadcare-uploader',
			));
			break;
	}
	return $element;
}

/**
 * Get a prefix with script
 */
function uplodcare_field_widget_form_prefix()
{
	$public_key = variable_get('uploadcare_public_key');
	$secret_key = variable_get('uploadcare_secret_key');
	$api = new Uploadcare_Api($public_key, $secret_key);
	return $api->widget->getScriptTag()."<script>jQuery(function() {jQuery('input[role=uploadcare-uploader]').hide();});</script>";
}

/**
 * Implements hook_field_is_empty().
 */
function uploadcare_field_is_empty($item, $field) {
	if (empty($item['uploadcare'])) {
		return TRUE;
	}
}

function uploadcare_get_view_html($file_id, $settings)
{
	if (!$file_id) {
		return '';
	}
	$public_key = variable_get('uploadcare_public_key');
	$secret_key = variable_get('uploadcare_secret_key');
	$api = new Uploadcare_Api($public_key, $secret_key);	
	
	$is_image = $settings['is_image'];
	
	$crop = isset($settings['crop']) ? $settings['crop'] : false;
	$crop_width = isset($settings['crop_width']) ? $settings['crop_width'] : null;
	$crop_height = isset($settings['crop_height']) ? $settings['crop_height'] : null;
	$crop_center = isset($settings['crop_center']) ? $settings['crop_center'] : null;
	$crop_fill_color = isset($settings['crop_fill_color']) ? $settings['crop_fill_color'] : false;
	
	$resize = isset($settings['resize']) ? $settings['resize'] : false;
	$resize_width = isset($settings['resize_width']) ? $settings['resize_width'] : null;
	$resize_height = isset($settings['resize_height']) ? $settings['resize_width'] : null;
	
	$scale_crop = isset($settings['scale_crop']) ? $settings['scale_crop'] : false;
	$scale_crop_width = isset($settings['scale_crop_width']) ? $settings['scale_crop_width'] : null;
	$scale_crop_height = isset($settings['scale_crop_height']) ? $settings['scale_crop_height'] : null;
	$scale_crop_center = isset($settings['scale_crop_center']) ? $settings['scale_crop_center'] : false;
	
	$flip = isset($settings['flip']) ? $settings['flip'] : false;
	$grayscale = isset($settings['grayscale']) ? $settings['grayscale'] : false;
	$invert = isset($settings['invert']) ? $settings['invert'] : false;
	$mirror = isset($settings['mirror']) ? $settings['mirror'] : false;
	$stretch_off = isset($settings['stretch_off']) ? $settings['stretch_off'] : false;	
	
	$file = $api->getFile($file_id);
	$result = db_query('SELECT filename FROM {uploadcare} where file_id = :file_id', array(':file_id' => $file_id));
	foreach ($result as $record) {
		$filename = $record->filename;
	}
	
	if ($is_image) {
		$original_file = clone $file;
		if ($crop) {
			$file = $file->crop($crop_width, $crop_height, $crop_center, $crop_fill_color);
		}
		if ($resize) {
			$file = $file->resize($resize_width, $resize_height);
		}
		if ($scale_crop) {
			$file = $file->scaleCrop($scale_crop_width, $scale_crop_height, $scale_crop_center);
		}
		if ($flip) {
			$file = $file->effect('flip');
		}
		if ($grayscale) {
			$file = $file->effect('grayscale');
		}
		if ($invert) {
			$file = $file->effect('invert');
		}
		if ($mirror) {
			$file = $file->effect('mirror');
		}
		if ($stretch_off) {
			$file = $file->op('stretch/off');
		}
		$html = sprintf('<a href="%s">%s</a>', $file->getUrl($filename), $file->getImgTag($filename));
	} else {
		$html = sprintf('<a href="%s">%s</a>', $file->getUrl($filename), $filename);
	}
	return $html;
}

/**
 * Implements hook_field_formatter_view().
 */
function uploadcare_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display)
{
	$settings = $field['settings'];
		
	$elements = array();
	foreach ($items as $delta => $item) {
		$file_id = $item['uploadcare'];

		$html = uploadcare_get_view_html($file_id, $settings);
		
		$elements[$delta] = array(
				'#markup' => $html,
		);
	}
	return $elements;
}